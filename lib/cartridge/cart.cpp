#include <cartridge/cart.h>
#include <cartridge/debug.h>
#include <cartridge/read_file.h>

static cart_context ctx;

bool cart_load(char *cart_path) {
  read_rom_file(cart_path, ctx);
  print_cart_header(ctx);
  return true;
}

BYTE cart_read(WORD address) {
  // for now just ROM ONLY supported
  return ctx.rom_data[address];
}

void cart_write(WORD address, BYTE value) {
  // for now just ROM ONLY supported
  std::cout << "CART write not implemented yet\n";
}
