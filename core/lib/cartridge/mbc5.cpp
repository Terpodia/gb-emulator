#include "cartridge/mbc1.h"
#include "cartridge/cart.h"

BYTE mbc5_read(WORD address){
  cart_context *ctx = cart_get_context();
  if(address < 0x4000) return ctx->rom_data[address];
  if(address >= 0x4000 && address <= 0x7FFF) 
    return ctx->current_rom_bank[address - 0x4000];
  if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx->ram_enabled) return 0xFF;
    return ctx->current_ram_bank[address - 0xA000];
  }
  return 0xFF;
}

void mbc5_write(WORD address, BYTE value){
  cart_context *ctx = cart_get_context();
  if(address <= 0x1FFF) ctx->ram_enabled = value== 0xA;
  else if(address >= 0x2000 && address <= 0x2FFF){
    ctx->rom_bank_low_bits = value;
    int bank = ((int)ctx->rom_bank_upper_bits << 9) | (int)ctx->rom_bank_low_bits;
    ctx->current_rom_bank = ctx->rom_data + 0x4000 * bank;
  }
  else if(address >= 0x3000 && address <= 0x3FFF){
    ctx->rom_bank_upper_bits = value & 1;
    int bank = ((int)ctx->rom_bank_upper_bits << 9) | (int)ctx->rom_bank_low_bits;
    ctx->current_rom_bank = ctx->rom_data + 0x4000 * bank;
  }
  else if(address >= 0x4000 && address <= 0x5FFF){
    if(value <= 0xF) ctx->ram_bank_value = value;
    ctx->current_ram_bank = ctx->ram_bank[ctx->ram_bank_value];
  }
  else if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx->ram_enabled) return;
    ctx->current_ram_bank[address - 0xA000] = value;
    if(has_battery()) ctx->should_save_battery = true;
  }
}
