#include <mbc1.h>
#include <cartridge/cart.h>

void check_multicart(){
  cart_context *ctx = cart_get_context();

  if(ctx->header->rom_size < 0x5) return;

  for(int bank = 0; bank < 4; bank++){
    bool ok = true;
    for(int i = 0x104; i <= 0x133; i++){
      if(ctx->rom_data[i + bank * 0x4000] != ctx->rom_data[i]) ok=false;
    }
    if(ok) ctx->is_multicart = true;
  }
}

int hi_shift(){
  cart_context *ctx = cart_get_context();
  if(ctx->is_multicart) return 4;
  return 5;
}

void change_rom_bank(){
  cart_context *ctx = cart_get_context();

  int idx = ctx->rom_bank_low_bits;
  if(ctx->header->rom_size >= 0x5){
    idx += ctx->rom_bank_upper_bits << hi_shift();
  }

  idx &= (1 << (ctx->header->rom_size + 1)) - 1;
  ctx->current_rom_bank = ctx->rom_data + 0x4000 * idx;
}

BYTE mbc1_read(WORD address){
  cart_context *ctx = cart_get_context();
  if(address < 0x4000){
    if(ctx->bank_mode && ctx->header->rom_size >= 0x5){
      int idx = ctx->rom_bank_upper_bits << hi_shift();
      idx &= (1 << (ctx->header->rom_size + 1)) - 1;

      return ctx->rom_data[idx * 0x4000 + address];
    }

    return ctx->rom_data[address];
  }
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

    if(ctx->is_multicart) value &= ~(1<<4);

    ctx->rom_bank_low_bits = value;
    change_rom_bank();
  }

  else if(address >= 0x4000 && address <= 0x5FFF){
    value &= 0b11;
    if(ctx->header->rom_size >= 0x5) ctx->rom_bank_upper_bits = value;
    else ctx->ram_bank_value = value;

    if(ctx->bank_mode)
      ctx->current_ram_bank = ctx->ram_bank[ctx->ram_bank_value];

    change_rom_bank();
  }

  else if(address >= 0x6000 && address <= 0x7FFF){
    ctx->bank_mode = value & 1;
    if(ctx->bank_mode)
      ctx->current_ram_bank = ctx->ram_bank[ctx->ram_bank_value];
    else
      ctx->current_ram_bank = ctx->ram_bank[0];
  }

  else if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx->ram_enabled) return;
    ctx->current_ram_bank[address - 0xA000] = value;
    if(has_battery()) ctx->should_save_battery = true;
  }
}

