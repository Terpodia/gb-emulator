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

bool window_is_visible(){
  return ctx.wx < XRES + 7 && ctx.wy < YRES && WIN_ENABLE;
}

