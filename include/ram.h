#pragma once

#include <common.h>

struct ram_context {
    BYTE wram[0x2000];
    BYTE hram[0x80];
};

BYTE wram_read(WORD address);
void wram_write(WORD address, BYTE value);

BYTE hram_read(WORD address);
void hram_write(WORD address, BYTE value);
