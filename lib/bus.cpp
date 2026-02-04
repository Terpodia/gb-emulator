#include <bus.h>
#include <cartridge/cart.h>

BYTE bus_read(WORD address) {
  // for now ROM ONLY supported
  return cart_read(address);
}

void bus_write(WORD address, BYTE value) {
  // for now ROM ONLY supported
  cart_write(address, value);
}

void bus_write16(WORD address, WORD value) {
  cart_write(address, value & 0xFF);
  cart_write(address + 1, value >> 8);
}
