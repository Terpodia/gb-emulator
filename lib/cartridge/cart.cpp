#include <cartridge/cart.h>
#include <cartridge/debug.h>
#include <cartridge/read_file.h>
#include <mbc1.h>
#include <mbc2.h>
#include <mbc5.h>
#include <cstring>

static cart_context ctx;

bool is_mbc1(){
  return ctx.header->type >= 1 && ctx.header->type <= 3;
}
bool is_mbc2(){
  return ctx.header->type >= 5 && ctx.header->type <= 6;
}
bool is_mbc5(){
  return ctx.header->type >= 0x19 && ctx.header->type <= 0x1E;
}
bool has_battery(){
  return ctx.header->type == 3 || ctx.header->type == 6 || ctx.header->type == 0x1B || ctx.header->type == 0x1E;
}

void initialize_banking(){
  if(!ctx.header->type) return;

  ctx.ram_banks = 1;
  if(ctx.header->ram_size == 2) ctx.ram_banks = 1;
  else if(ctx.header->ram_size == 3) ctx.ram_banks = 4;
  else if(ctx.header->ram_size == 4) ctx.ram_banks = 16;
  else if(ctx.header->ram_size == 5) ctx.ram_banks= 8;

  for(int i = 0; i < ctx.ram_banks; i++){
    ctx.ram_bank[i] = (BYTE *) malloc(0x2000);
    memset(ctx.ram_bank[i], 0, 0x2000);
  }

  ctx.current_rom_bank = ctx.rom_data + 0x4000;
  ctx.current_ram_bank = ctx.ram_bank[0];
  ctx.should_save_battery = false;
}

static BYTE save_buffer[16 * 0x2000];

void save_battery(){
  for(int i = 0; i < ctx.ram_banks; i++)
    for(int j = 0; j < 0x2000; j++) 
      save_buffer[j + i * 0x2000] = ctx.ram_bank[i][j];

  char filename[1024];
  snprintf(filename, sizeof(ctx.filename), "%s.battery", ctx.filename);
  FILE *fp = fopen(filename, "wb");
  fwrite(save_buffer, 0x2000 * ctx.ram_banks, 1, fp);
  fclose(fp);
  ctx.should_save_battery = false;
}

void battery_load(){
  char filename[1024];
  snprintf(filename, sizeof(ctx.filename), "%s.battery", ctx.filename);
  FILE *fp = fopen(filename, "rb");
  if(!fp) return;

  fread(save_buffer, 0x2000 * ctx.ram_banks, 1, fp);
  for(int i = 0; i < ctx.ram_banks; i++)
    for(int j = 0; j < 0x2000; j++) 
      ctx.ram_bank[i][j] = save_buffer[j + i * 0x2000];
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
  if(is_mbc5()) return mbc5_read(address);
  return ctx.rom_data[address];
}

void cart_write(WORD address, BYTE value) {
  if(is_mbc1()) mbc1_write(address, value);
  else if(is_mbc2()) mbc2_write(address, value);
  else if(is_mbc5()) mbc5_write(address, value);
}

cart_context *cart_get_context(){
  return &ctx;
}
