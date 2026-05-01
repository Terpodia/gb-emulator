#include "cartridge/cart.h"
#include "cartridge/read_file.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <iomanip>
#include <sstream>

bool read_rom_file(char *cart_path, cart_context &ctx) {
  SDL_IOStream *rom = SDL_IOFromFile(cart_path, "rb");
  snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart_path);
  if (!rom) {
    SDL_Log("failed to open %s", cart_path);
    return false;
  }
  SDL_Log("Opened: %s", ctx.filename);

  ctx.rom_size = SDL_GetIOSize(rom);

  ctx.rom_data = (BYTE *)malloc(ctx.rom_size);

  SDL_ReadIO(rom, ctx.rom_data, ctx.rom_size);

  SDL_CloseIO(rom);

  ctx.header = (rom_header *)(ctx.rom_data + 0x100);
  ctx.cgb_mode = ctx.header->title[15] & 0x80;
  ctx.header->title[15] = 0;

  SDL_Log("%s", ctx.header->title);

  return true;
}
