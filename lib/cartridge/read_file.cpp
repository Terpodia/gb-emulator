#include <sstream>
#include <iomanip>
#include <cartridge/cart.h>

bool read_rom_file(char *cart_path, cart_context &ctx) {
  snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart_path);
  FILE *fp = fopen(cart_path, "r");
  if (!fp) {
    std::cout << "Failed to open: " << cart_path << "\n";
    return false;
  }
  std::cout << "Opened: " << ctx.filename << "\n";

  fseek(fp, 0, SEEK_END);
  ctx.rom_size = ftell(fp);

  rewind(fp);

  ctx.rom_data = (BYTE*)malloc(ctx.rom_size);
  fread(ctx.rom_data, ctx.rom_size, 1, fp);
  fclose(fp);

  ctx.header = (rom_header*)(ctx.rom_data + 0x100);
  ctx.header->title[15] = 0;

  return true;
}
