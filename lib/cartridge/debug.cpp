#include <cartridge/cart.h>
#include <cartridge/constants.h>
#include <cartridge/logo_utils.h>

const char *cart_lic_name(cart_context &ctx) {
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

const char *cart_type_name(BYTE type) {
  if (type < 0x23) return ROM_TYPES[type];
  return "UNKNOWN";
}

WORD get_checksum(BYTE *rom_data) {
  WORD checksum = 0;
  for (WORD i = 0x0134; i <= 0x014C; i++) checksum = checksum - rom_data[i] - 1;
  return checksum;
}

void print_checksum(cart_context &ctx) {
  WORD checksum = get_checksum(ctx.rom_data);
  std::string status = checksum & 0xFF ? " (PASSED)\n" : " (FAILED)\n";
  std::cout << "\t CHECKSUM    : " << (int)ctx.header->checksum << status;
}

void print_cart_header(cart_context &ctx) {
  std::cout << std::hex;
  std::cout << "Cartridge Loaded:\n\n";
  decode_and_print_logo(ctx.header->logo);
  std::cout << "\n";
  std::cout << "\t TITLE       : " << ctx.header->title << "\n";
  std::cout << "\t TYPE        : " << (int)ctx.header->type << " "
            << " (" << cart_type_name(ctx.header->type) << ")\n";
  std::cout << "\t LIC_CODE    : " << (int)ctx.header->licensee_code << " "
            << " (" << cart_lic_name(ctx) << ")\n";
  std::cout << "\t ROM SIZE    : " << std::dec << (32 << ctx.header->rom_size)
            << " KB\n"
            << std::hex;
  std::cout << "\t RAM SIZE    : " << (int)ctx.header->ram_size << "\n";
  std::cout << "\t ROM VERSION : " << (int)ctx.header->version << "\n";
  print_checksum(ctx);
  std::cout << std::dec;
}
