#include "cartridge/save_data.h"
#include "cartridge/cart.h"

static BYTE save_buffer[16 * 0x2000 + 100];

void save_battery(){
  cart_context *ctx = cart_get_context();
  //TODO
}

void battery_load(){
  cart_context *ctx = cart_get_context();
  //TODO
}
