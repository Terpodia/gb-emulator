#pragma once

#include <common.h>

struct cgb_palettes_ctx {
  BYTE bcps;
  BYTE ocps;
  BYTE cgb_bg_palette[64];
  BYTE cgb_obj_palette[64];
};

#define PALETTES 8
#define COLORS_PER_PALETTE 4
#define BYTES_PER_COLOR 2

BYTE cgb_palette_read(WORD address);
void cgb_palette_write(WORD address, BYTE value);

uint32_t bg_rgb_color_from_palette(BYTE palette_index, BYTE offset);
uint32_t obj_rgb_color_from_palette(BYTE palette_index, BYTE offset);

