#include <mbc1.h>
#include <cartridge/cart.h>

BYTE mbc1_read(WORD address){
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

void mbc1_write(WORD address, BYTE value){
  cart_context *ctx = cart_get_context();

  if(address <= 0x1FFF) ctx->ram_enabled = (value & 0xF) == 0xA;

  else if(address >= 0x2000 && address <= 0x3FFF){
    value &= 0b11111;
    value += value == 0;
    value &= (1 << (ctx->header->rom_size + 2)) - 1;

    ctx->rom_bank_low_bits = value;
    if(!ctx->bank_mode) value |= ctx->rom_bank_upper_bits << 5;
    ctx->current_rom_bank = ctx->rom_data + 0x4000 * (int)value;
  }

  else if(address >= 0x4000 && address <= 0x5FFF){
    value &= 0b11;
    if(ctx->bank_mode){
      ctx->ram_bank_value = value;
      ctx->current_ram_bank = ctx->ram_bank[ctx->ram_bank_value];
    }
    else{
      ctx->rom_bank_upper_bits = value;
      ctx->current_rom_bank = ctx->rom_data + 0x4000 * (int)(ctx->rom_bank_low_bits | (value << 5));
    }
  }

  else if(address >= 0x6000 && address <= 0x7FFF){
    ctx->bank_mode = value & 1;
    if(ctx->bank_mode){ 
      ctx->current_ram_bank = ctx->ram_bank[ctx->ram_bank_value];
      ctx->current_rom_bank = ctx->rom_data + 0x4000 * (int)ctx->rom_bank_low_bits;
    }
    else{
      ctx->current_ram_bank = ctx->ram_bank[0];
      ctx->current_rom_bank = ctx->rom_data + 0x4000 * (int)(ctx->rom_bank_low_bits | (ctx->rom_bank_upper_bits << 5));
    }
  }

  else if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx->ram_enabled) return;
    ctx->current_ram_bank[address - 0xA000] = value;
    if(has_battery()) ctx->should_save_battery = true;
  }
}
