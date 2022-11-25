#pragma once

#include <common.h>

struct rom_header {
  BYTE entry[4];
  BYTE logo[0x30];
  char title[0x10];
  WORD new_licensee_code;
  BYTE sgb_flag;
  BYTE type;
  BYTE rom_size;
  BYTE ram_size;
  BYTE destination_code; 
  BYTE licensee_code;
  BYTE version;
  BYTE checksum;
  WORD global_checksum;
};

struct cart_context {
  char filename[1024];
  unsigned int rom_size;
  BYTE *rom_data;
  rom_header *header;
};

bool cart_load(char *cart);

BYTE cart_read(WORD address);

void cart_write(WORD address, BYTE value);
