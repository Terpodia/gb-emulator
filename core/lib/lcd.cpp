#include "lcd.h"
#include "dma.h"
#include "ppu.h"
#include "ppu_sm.h"

static uint32_t default_color[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

static lcd_context ctx;

lcd_context *lcd_get_context() {
  return &ctx;
}

void lcd_init(){
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
    ctx.bg_colors[i] = default_color[i];
    ctx.sp1_colors[i] = default_color[i];
    ctx.sp2_colors[i] = default_color[i];
  }
  ctx.off_clock = 0;
}

BYTE lcd_read(WORD address){
  BYTE *p = (BYTE *)&ctx;
  BYTE value = p[address - 0xFF40];
  if(address == 0xFF41) value |= 0x80;
  return value;
}

void update_palette(WORD address, BYTE pal){
  uint32_t *colors = ctx.bg_colors;
  if(address == 0xFF48) colors = ctx.sp1_colors;
  else if(address == 0xFF49) colors = ctx.sp2_colors;

  for(int i=0; i<4; i++){
    colors[i] = default_color[pal & 0b11];
    pal >>= 2;
  }
}

void lcd_write(WORD address, BYTE value){
  if(address == 0xFF40){
    if((value & 0x80) && !(ctx.lcdc & 0x80)) {
      SET_LCD_MODE(MODE_OAM); 
      ppu_get_context()->ppu_ticks = 0;
      compare_lyc();
    }
    if(!(value & 0x80) && (ctx.lcdc & 0x80)) {
      SET_LCD_MODE(MODE_HBLANK); 
      ctx.ly = 0;
    }
  }
  if(address == 0xFF41){
    ctx.lcds = (value & 0x78) | (ctx.lcds & 0x07);
    return;
  }
  if(address == 0xFF46) dma_start(value);
  if(address == 0xFF47) update_palette(address, value);
  if(address == 0xFF48 || address == 0xFF49)
    update_palette(address, value & 0b11111100);

  BYTE *p = (BYTE *)&ctx;
  p[address - 0xFF40] = value;
}

