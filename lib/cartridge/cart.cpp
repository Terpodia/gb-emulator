#include <cartridge/cart.h>
#include <cartridge/debug.h>
#include <cartridge/read_file.h>
#include <mbc1.h>
#include <mbc2.h>
#include <cstring>

static cart_context ctx;

bool is_mbc1(){
  return ctx.header->type >= 1 && ctx.header->type <= 3;
}
bool is_mbc2(){
  return ctx.header->type >= 5 && ctx.header->type <= 6;
}
bool has_battery(){
  return ctx.header->type == 0x3;
}

void initialize_banking(){
  if(!ctx.header->type) return;

  int ram_banks = 1;
  if(ctx.header->ram_size == 2) ram_banks = 1;
  else if(ctx.header->ram_size == 3) ram_banks = 4;
  else if(ctx.header->ram_size == 4) ram_banks = 16;
  else if(ctx.header->ram_size == 5) ram_banks= 8;

  for(int i = 0; i < ram_banks; i++){
    ctx.ram_bank[i] = (BYTE *) malloc(0x2000);
    memset(ctx.ram_bank[i], 0, 0x2000);
  }

  ctx.current_rom_bank = ctx.rom_data + 0x4000;
  ctx.current_ram_bank = ctx.ram_bank[0];
}

void save_battery(){
  char filename[1024];
  snprintf(filename, sizeof(ctx.filename), "%s.battery", ctx.filename);
  FILE *fp = fopen(filename, "wb");
  fwrite(ctx.current_ram_bank, 0x2000, 1, fp);
  fclose(fp);
}

void battery_load(){
  char filename[1024];
  snprintf(filename, sizeof(ctx.filename), "%s.battery", ctx.filename);
  FILE *fp = fopen(filename, "rb");
  if(!fp) return;

  fread(ctx.current_ram_bank, 0x2000, 1, fp);
  fclose(fp);
}

bool cart_load(char *cart_path) {
  read_rom_file(cart_path, ctx);
  print_cart_header(ctx);
  return true;
}

BYTE cart_read(WORD address) {
  if(is_mbc1()) return mbc1_read(address);
  if(is_mbc2()) return mbc2_read(address);
  return ctx.rom_data[address];
}

void cart_write(WORD address, BYTE value) {
  if(is_mbc1()) mbc1_write(address, value);
  else if(is_mbc2()) mbc2_write(address, value);
}

cart_context *cart_get_context(){
  return &ctx;
}
