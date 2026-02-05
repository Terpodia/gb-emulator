#include <ram.h>

static ram_context ctx;

BYTE wram_read(WORD address){
    address -= 0xC000;
    return ctx.wram[address];
} 
void wram_write(WORD address, BYTE value){
    address -= 0xC000;
    ctx.wram[address] = value;
}

BYTE hram_read(WORD address){
    address -= 0xFF80;
    return ctx.hram[address];
}
void hram_write(WORD address, BYTE value){
    address -= 0xFF80;
    ctx.hram[address] = value;
}
