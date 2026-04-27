#include "mbc3.h"
#include "cartridge/cart.h"

void latch_time(){
  cart_context *ctx = cart_get_context();
  uint64_t nxt_time = time(NULL);
  if(nxt_time < ctx->current_time){
    ctx->current_time = nxt_time;
    return;
  }
  if(BIT(ctx->rtc_register[4], 6)) return;

  uint64_t diff = nxt_time - ctx->current_time;
  ctx->current_time = nxt_time;

  uint64_t sec = 0, min = 0, hour = 0, days = 0;

  sec = ctx->rtc_register[0];
  sec += diff;
  ctx->rtc_register[0] = sec % 60;

  min = ctx->rtc_register[1];
  min += sec / 60;
  ctx->rtc_register[1] = min % 60;

  hour = ctx->rtc_register[2];
  hour += min / 60;
  ctx->rtc_register[2] = hour % 24;

  days = ctx->rtc_register[3];
  days |= (uint64_t)(ctx->rtc_register[4] & 1) << (uint64_t)8;

  uint64_t cnt = hour / 24;
  BYTE carry = days + cnt >= (1 << 9);

  days += cnt, days %= 1<<9;
  ctx->rtc_register[3] = days & 0xFF;

  BIT_SET(ctx->rtc_register[4], 0, days >> 8);
  BIT_SET(ctx->rtc_register[4], 7, carry);

  if(has_battery()) ctx->should_save_battery = true;
}

BYTE mbc3_read(WORD address){
  cart_context *ctx = cart_get_context();
  if(address < 0x4000) return ctx->rom_data[address];
  if(address >= 0x4000 && address <= 0x7FFF) 
    return ctx->current_rom_bank[address - 0x4000];
  if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx->ram_enabled) return 0xFF;
    if(!ctx->current_rtc_register) return ctx->current_ram_bank[address - 0xA000];
    return ctx->rtc_register[ctx->current_rtc_register - 0x8];
  }
  return 0xFF;
}

void mbc3_write(WORD address, BYTE value){
  cart_context *ctx = cart_get_context();
  if(address <= 0x1FFF) ctx->ram_enabled = (value & 0xF) == 0xA; 
  else if(address >= 0x2000 && address <= 0x3FFF){
    int offset = value & 0x7F;
    if(!offset) offset++;
    offset &= (1 << (ctx->header->rom_size + 1)) - 1;
    ctx->current_rom_bank = ctx->rom_data + 0x4000 * offset;
  }
  else if(address >= 0x4000 && address <= 0x5FFF){
    if(value <= 0x7) ctx->current_ram_bank = ctx->ram_bank[value], ctx->current_rtc_register = 0;
    else ctx->current_rtc_register = value;
  }
  else if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx->ram_enabled) return;

    if(!ctx->current_rtc_register) ctx->current_ram_bank[address - 0xA000] = value;
    else ctx->rtc_register[ctx->current_rtc_register - 0x8] = value;

    if(has_battery()) ctx->should_save_battery = true;
  }
  else if(address >= 0x6000 && address <= 0x7FFF){
    if(ctx->should_latch_time && value == 0x1) latch_time();
    ctx->should_latch_time = value == 0;
  }
}
