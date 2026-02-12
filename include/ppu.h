#pragma once

#include <common.h>

struct oam_entry {
  BYTE y;
  BYTE x;
  BYTE tile_index;
  BYTE cgb_palette : 3;
  BYTE vram_back : 1;
  BYTE dmg_palette : 1;
  BYTE x_flip : 1;
  BYTE y_flip : 1;
  BYTE priority : 1;
};

struct ppu_context {
  oam_entry oam[40];
  BYTE vram[0x2000];
};

BYTE ppu_oam_read(WORD address);
void ppu_oam_write(WORD address, BYTE value);

BYTE ppu_vram_read(WORD address);
void ppu_vram_write(WORD address, BYTE value);
