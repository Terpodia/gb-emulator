#include "ram.h"

static ram_context ram_ctx;

BYTE get_wram_bank(){
  return ram_ctx.wram_bank;
}
void set_wram_bank(int bank){
  bank &= 0x7;
  if(bank == 0) bank++;
  ram_ctx.wram_bank = bank;
}

BYTE wram_read(WORD address){
  address -= 0xC000;
  if(address < 0x1000) return ram_ctx.wram[address];

  address -= 0x1000;
  return ram_ctx.wram[address + ram_ctx.wram_bank * 0x1000];
} 
void wram_write(WORD address, BYTE value){
  address -= 0xC000;
  if(address < 0x1000){
    ram_ctx.wram[address] = value;
    return;
  }
  address -= 0x1000;
  ram_ctx.wram[address + ram_ctx.wram_bank * 0x1000] = value;
}

BYTE hram_read(WORD address){
  address -= 0xFF80;
  return ram_ctx.hram[address];
}
void hram_write(WORD address, BYTE value){
  address -= 0xFF80;
  ram_ctx.hram[address] = value;
}

