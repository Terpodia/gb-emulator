#pragma once

#include <common.h>

struct lcd_context {
  BYTE lcdc;
  BYTE lcds;
  BYTE scy;
  BYTE scx;
  BYTE ly;
  BYTE lyc;
  BYTE dma;
  BYTE bg_palette;
  BYTE obj_palette[2];
  BYTE wy;
  BYTE wx;

  uint32_t bg_colors[4];
  uint32_t sp1_colors[4];
  uint32_t sp2_colors[4];
};

enum lcd_mode {
  MODE_HBLANK,
  MODE_VBLANK,
  MODE_OAM,
  MODE_PIXEL_TRANSFER
};

enum stat_int_mode {
  STAT_INT_MODE_LYC = 1 << 6,
  STAT_INT_MODE_OAM = 1 << 5,
  STAT_INT_MODE_VBLANK = 1 << 4,
  STAT_INT_MODE_HBLANK = 1 << 3
};

lcd_context *lcd_get_context();

#define LCD_MODE (lcd_mode)(lcd_get_context()->lcds & 0b11)
#define SET_LCD_MODE(mode) { lcd_get_context()->lcds &= ~(0b11); lcd_get_context()->lcds |= mode; }

#define LCDS_LYC_SET(value) BIT_SET(lcd_get_context()->lcds, 2, value)

#define LCDS_STAT_INT(mode) (lcd_get_context()->lcds & mode)

#define BG_TILE_MAP_AREA (BIT(lcd_get_context()->lcdc, 3) ? 0x9C00 : 0x9800)

#define BGW_TILE_DATA_AREA (BIT(lcd_get_context()->lcdc, 4) ? 0x8000 : 0x8800)

#define BGW_ENABLE (BIT(lcd_get_context()->lcdc, 0))

void lcd_init();

BYTE lcd_read(WORD address);
void lcd_write(WORD address, BYTE value);
