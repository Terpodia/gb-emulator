#include <ppu.h>
#include <ppu_sm.h>
#include <lcd.h>
#include <cstring>

static ppu_context ctx;

ppu_context *ppu_get_context(){
  return &ctx;
}

void ppu_init(){
  lcd_init();
  SET_LCD_MODE(MODE_OAM);
  ctx.ppu_ticks = 0;
  memset(ctx.video_buffer, 0, sizeof(ctx.video_buffer));
}

void ppu_tick(){
  ctx.ppu_ticks++;

  switch(LCD_MODE){
    case MODE_OAM:
      ppu_mode_oam();
      break;

    case MODE_PIXEL_TRANSFER:
      ppu_mode_pixel_transfer();
      break;

    case MODE_HBLANK:
      ppu_mode_hblank();
      break;

    case MODE_VBLANK:
      ppu_mode_vblank();
      break;
  }
}

BYTE ppu_oam_read(WORD address){
  BYTE *oam = (BYTE *)ctx.oam;
  return oam[address - 0xFE00];
}
void ppu_oam_write(WORD address, BYTE value){
  BYTE *oam = (BYTE *)ctx.oam;
  oam[address - 0xFE00] = value;
}

BYTE ppu_vram_read(WORD address){
  return ctx.vram[address - 0x8000];
}
void ppu_vram_write(WORD address, BYTE value){
  ctx.vram[address - 0x8000] = value;
}
