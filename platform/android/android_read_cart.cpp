#include "cartridge/cart.h"
#include "cartridge/read_file.h"

#include <SDL.h>
#include <iomanip>
#include <sstream>

bool read_rom_file(char *cart_path, cart_context &ctx) {
  SDL_RWops *rom = SDL_RWFromFile(cart_path, "rb");
  snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart_path);
  if (!rom) {
    std::cout << "Failed to open: " << cart_path << "\n";
    return false;
  }
  std::cout << "Opened: " << ctx.filename << "\n";

  ctx.rom_size = SDL_RWsize(rom);

  ctx.rom_data = (BYTE *)malloc(ctx.rom_size);

  SDL_RWread(rom, ctx.rom_data, 1, ctx.rom_size);

  SDL_RWclose(rom);

  ctx.header = (rom_header *)(ctx.rom_data + 0x100);
  ctx.header->title[15] = 0;

  return true;
}
