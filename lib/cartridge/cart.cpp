#include <cartridge/cart.h>
#include <cartridge/constants.h>
#include <cartridge/logo_utils.h>
#include <sstream>
#include <iomanip>

static cart_context ctx;

const char *cart_lic_name() {
  const bool is_unknown_lic = ctx.header->licensee_code >= 0xA5;
  if (is_unknown_lic) return "UNKNOWN";

  const bool use_new_lic = ctx.header->licensee_code == 0x33;
  if (use_new_lic) {
    int ascii_hi = *(ctx.rom_data + 0x144);
    int ascii_low = *(ctx.rom_data + 0x145);
    int hex_hi = (ascii_hi >= 65) ? ascii_hi - 55 : ascii_hi - 48;
    int hex_low = (ascii_low >= 65) ? ascii_low - 55 : ascii_low - 48;
    return NEW_LIC_CODE[hex_hi * 16 + hex_low];
  }
  return LIC_CODE[ctx.header->licensee_code];
}

const char *cart_type_name() {
  if (ctx.header->type < 0x23)
    return ROM_TYPES[ctx.header->type];
  return "UNKNOWN";
}

bool cart_load(char *cart_path) {
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

  std::cout << std::hex;

  std::cout << "Cartridge Loaded:\n\n";
  decode_and_print_logo(ctx.header->logo);
  std::cout << "\n";
  std::cout << "\t TITLE       : " << ctx.header->title << "\n";
  std::cout << "\t TYPE        : " << (int)ctx.header->type << " " << " (" << cart_type_name() << ")\n";
  std::cout << "\t LIC_CODE    : " << (int)ctx.header->licensee_code << " " << " (" << cart_lic_name() << ")\n";
  std::cout << "\t ROM SIZE    : " << std::dec << (32 << ctx.header->rom_size) << " KB\n" << std::hex; 
  std::cout << "\t RAM SIZE    : " << (int)ctx.header->ram_size << "\n"; 
  std::cout << "\t ROM VERSION : " << (int)ctx.header->version << "\n";

  WORD checksum = 0;
  for (WORD i=0x0134; i<=0x014C; i++) 
      checksum = checksum - ctx.rom_data[i] - 1;

  if (checksum & 0xFF) 
    std::cout << "\t CHECKSUM    : " << (int)ctx.header->checksum << " (PASSED)\n";
  else 
    std::cout << "\t CHECKSUM    : " << (int)ctx.header->checksum << " (FAILED)\n";

  std::cout << std::dec;
  
  return true;
}

BYTE cart_read(WORD address){
  // for now just ROM ONLY supported
  return ctx.rom_data[address];
}

void cart_write(WORD address, BYTE value){
  // for now just ROM ONLY supported
    std::cout << "Not implemented yet\n";
}