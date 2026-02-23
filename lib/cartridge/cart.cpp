#include <cartridge/cart.h>
#include <cartridge/debug.h>
#include <cartridge/read_file.h>
#include <cstring>

static cart_context ctx;

bool is_mcb1(){
  return ctx.header->type >= 1 && ctx.header->type <= 3;
}
bool has_battery(){
  return ctx.header->type == 0x3;
}

void initialize_banking(){
  if(!is_mcb1()) return;

  int ram_banks = 0;
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
  ctx.ram_bank_value = 0;
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
  if(!is_mcb1() || address < 0x4000) return ctx.rom_data[address];
  if(address >= 0x4000 && address <= 0x7FFF) return ctx.current_rom_bank[address - 0x4000];
  if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx.ram_enabled) return 0xFF;
    return ctx.current_ram_bank[address - 0xA000];
  }
  return 0xFF;
}

void cart_write(WORD address, BYTE value) {
  if(!is_mcb1()) return;

  if(address <= 0x1FFF){
    if((value & 0xF) == 0xA) ctx.ram_enabled = true;
    else ctx.ram_enabled = false;
  }

  else if(address >= 0x2000 && address <= 0x3FFF){
    value &= 0b11111;
    value += value == 0;
    value &= (1 << (ctx.header->rom_size + 2)) - 1;

    ctx.current_rom_bank = ctx.rom_data + 0x4000 * value;
  }

  else if(address >= 0x4000 && address <= 0x5FFF){
    value &= 0xFF;
    ctx.ram_bank_value = value;
    if(ctx.bank_mode)
      ctx.current_ram_bank = ctx.ram_bank[ctx.ram_bank_value];
  }

  else if(address >= 0x6000 && address <= 0x7FFF){
    ctx.bank_mode = value & 1;
    if(ctx.bank_mode) ctx.current_ram_bank = ctx.ram_bank[ctx.ram_bank_value];
  }

  else if(address >= 0xA000 && address <= 0xBFFF){
    if(!ctx.ram_enabled) return;
    ctx.current_ram_bank[address - 0xA000] = value;
    if(has_battery()) save_battery();
  }
}
