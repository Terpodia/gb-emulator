#include <ppu/lcd.h>
#include <ppu/dma.h>
#include <ppu/ppu.h>
#include <ppu/ppu_sm.h>

static uint32_t dmg_default_color[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

static lcd_context ctx;

lcd_context *lcd_get_context() {
  return &ctx;
}

void lcd_init(BYTE cgb_mode){
  ctx.lcdc = 0x91;
  ctx.scx = 0;
  ctx.scy = 0;
  ctx.ly = 0;
  ctx.lyc = 0;
  ctx.bg_palette = 0xFC;
  ctx.obj_palette[0] = 0xFF;
  ctx.obj_palette[1] = 0xFF;
  ctx.wy = 0;
  ctx.wx = 0;
  for(int i=0; i<4; i++){
    ctx.dmg_bg_colors[i] = dmg_default_color[i];
    ctx.dmg_sp1_colors[i] = dmg_default_color[i];
    ctx.dmg_sp2_colors[i] = dmg_default_color[i];
  }
  ctx.off_clock = 0;
  ctx.object_priority_mode = !cgb_mode;
}

BYTE lcd_read(WORD address){
  BYTE *p = (BYTE *)&ctx;
  BYTE value = p[address - 0xFF40];
  if(address == 0xFF41) value |= 0x80;
  return value;
}

void dmg_update_palette(WORD address, BYTE pal){
  uint32_t *colors = ctx.dmg_bg_colors;
  if(address == 0xFF48) colors = ctx.dmg_sp1_colors;
  else if(address == 0xFF49) colors = ctx.dmg_sp2_colors;

  for(int i=0; i<4; i++){
    colors[i] = dmg_default_color[pal & 0b11];
    pal >>= 2;
  }
}

void lcd_write(WORD address, BYTE value){
  if(address == 0xFF40){
    if((value & 0x80) && !(ctx.lcdc & 0x80)) {
      SET_LCD_MODE(MODE_OAM);
      compare_lyc();
    }
    if(!(value & 0x80) && (ctx.lcdc & 0x80)) {
      SET_LCD_MODE(MODE_HBLANK);
      ctx.ly = 0;
      LCDS_LYC_SET(0);

      ppu_get_context()->window_line = 0;
      ppu_get_context()->ppu_ticks = 0;
    }
  }
  if(address == 0xFF41){
    ctx.lcds = (value & 0x78) | (ctx.lcds & 0x07);
    return;
  }
  if(address == 0xFF46) dma_start(value);
  if(address == 0xFF47) dmg_update_palette(address, value);
  if(address == 0xFF48 || address == 0xFF49)
    dmg_update_palette(address, value & 0b11111100);

  BYTE *p = (BYTE *)&ctx;
  p[address - 0xFF40] = value;
}

BYTE lcd_cgb_read(WORD address){
  switch(address){
    case 0xFF68: 
      return ctx.bcps;

    case 0xFF69: 
      return ctx.cgb_bg_palette[ctx.bcps & 0x3F];

    case 0xFF6A:
      return ctx.ocps;

    case 0xFF6B:
      return ctx.cgb_obj_palette[ctx.ocps & 0x3F];
  }
  return 0xFF;
}
void lcd_cgb_write(WORD address, BYTE value){
  switch(address){
    case 0xFF68: 
      ctx.bcps = value;
      break;

    case 0xFF69: 
      ctx.cgb_bg_palette[ctx.bcps & 0x3F] = value;
      if(BIT(ctx.bcps, 7)){
        ctx.bcps = (ctx.bcps & 0x3F) + 1;
        ctx.bcps &= 0x3F, ctx.bcps |= 1<<7;
      }
      break;

    case 0xFF6A:
      ctx.ocps = value;
      break;

    case 0xFF6B:
      ctx.cgb_obj_palette[ctx.ocps & 0x3F] = value;
      if(BIT(ctx.ocps, 7)){
        ctx.ocps = (ctx.ocps & 0x3F) + 1;
        ctx.ocps &= 0x3F, ctx.ocps |= 1<<7;
      }
      break;
  }
}
