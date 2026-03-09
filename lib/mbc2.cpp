#include <mbc2.h>
#include <cartridge/cart.h>

BYTE mbc2_read(WORD address){
  cart_context *ctx = cart_get_context();
  if(address < 0x4000) return ctx->rom_data[address];
  if(address >= 0x4000 && address <= 0x7FFF) 
    return ctx->current_rom_bank[address - 0x4000];
  if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx->ram_enabled) return 0xFF;
    return ctx->current_ram_bank[address & 0x1FF] & 0xF;
  }
  return 0xFF;
}

void mbc2_write(WORD address, BYTE value){
  cart_context *ctx = cart_get_context();
  if(address <= 0x3FFF){
    if(BIT(address, 8)){
      int offset = value & 0xF;
      if(!offset) offset++;
      ctx->current_rom_bank = ctx->rom_data + 0x4000 * offset;
    }
    else{
      ctx->ram_enabled = (value & 0xF) == 0xA;
    }
  }
  else if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx->ram_enabled) return;
    ctx->current_ram_bank[address & 0x1FF] = value & 0xF;
    if(has_battery()) save_battery();
  }
}
