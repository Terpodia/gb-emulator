#include <ram.h>

static ram_context ram_ctx;

BYTE wram_read(WORD address){
    address -= 0xC000;
    return ram_ctx.wram[address];
} 
void wram_write(WORD address, BYTE value){
    address -= 0xC000;
    ram_ctx.wram[address] = value;
}

BYTE hram_read(WORD address){
    address -= 0xFF80;
    return ram_ctx.hram[address];
}
void hram_write(WORD address, BYTE value){
    address -= 0xFF80;
    ram_ctx.hram[address] = value;
}
