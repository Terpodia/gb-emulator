#pragma once

#include <common.h>

struct ram_context {
  BYTE wram[0x1000 * 8];
  int wram_bank;

  BYTE hram[0x80];
  ram_context() : wram_bank(1) {}
};

BYTE get_wram_bank();
void set_wram_bank(int bank);

BYTE wram_read(WORD address);
void wram_write(WORD address, BYTE value);

BYTE hram_read(WORD address);
void hram_write(WORD address, BYTE value);
