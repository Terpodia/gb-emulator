#include "cartridge/save_data.h"
#include "cartridge/cart.h"
#include <SDL3/SDL.h>
#include <string.h>

static BYTE save_buffer[16 * 0x2000 + 100];

static void build_battery_path(char *out, size_t out_size, const char *rom_path){
  const char *base = rom_path;
  for(const char *p = rom_path; *p; p++){
    if(*p == '/' || *p == '%') base = p + 1;
  }
  snprintf(out, out_size, "%s/%s.battery", SDL_GetAndroidInternalStoragePath(), base);
}

void save_battery(){
  cart_context *ctx = cart_get_context();

  for(int i = 0; i < ctx->ram_banks; i++)
    for(int j = 0; j < 0x2000; j++)
      save_buffer[j + i * 0x2000] = ctx->ram_bank[i][j];

  for(int i = 0; i < 5; i++)
     save_buffer[i + ctx->ram_banks * 0x2000] = ctx->rtc_register[i];

  uint64_t current_time = ctx->current_time;
  for(int i = 0; i < 8; i++){
    save_buffer[5 + i + ctx->ram_banks * 0x2000] = current_time & 0xFF;
    current_time >>= 8;
  }

  char filename[1024];
  build_battery_path(filename, sizeof(filename), ctx->filename);

  SDL_IOStream *fp = SDL_IOFromFile(filename, "wb");
  SDL_WriteIO(fp, save_buffer, 0x2000 * ctx->ram_banks + 5 + 8);
  SDL_CloseIO(fp);

  ctx->should_save_battery = false;
}

void battery_load(){
  cart_context *ctx = cart_get_context();

  char filename[1024];
  build_battery_path(filename, sizeof(filename), ctx->filename);

  SDL_IOStream *fp = SDL_IOFromFile(filename, "rb");
  if(!fp) return;

  SDL_ReadIO(fp, save_buffer, 0x2000 * ctx->ram_banks + 5 + 8);

  for(int i = 0; i < ctx->ram_banks; i++)
    for(int j = 0; j < 0x2000; j++) 
      ctx->ram_bank[i][j] = save_buffer[j + i * 0x2000];

  for(int i = 0; i < 5; i++) 
    ctx->rtc_register[i] = save_buffer[i + ctx->ram_banks * 0x2000];

  for(int i = 0; i < 8; i++){
    ctx->current_time |= (uint64_t)save_buffer[5 + i + ctx->ram_banks * 0x2000] << (uint64_t)(i * 8);
  }
  SDL_CloseIO(fp);
}

