#include <ui.h>
#include <emu.h>
#include <bus.h>
#include <ppu.h>
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int SCALE = 4;
const int TILE_WIDTH = 8;
const int TILE_HEIGHT = 8;
const int TILE_SIZE = 16;

const int DEBUG_SCREEN_WIDTH = 24 * TILE_WIDTH * SCALE + 24 * SCALE;
const int DEBUG_SCREEN_HEIGHT = 16 * TILE_HEIGHT * SCALE + 16 * SCALE;

const int SCREEN_WIDTH = 160 * SCALE;
const int SCREEN_HEIGHT = 144 * SCALE;

SDL_Window *dbg_window = nullptr;
SDL_Renderer *dbg_renderer = nullptr;
SDL_Surface *dbg_screen_surface = nullptr;
SDL_Texture *dbg_texture = nullptr;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Surface *screen_surface = nullptr;
SDL_Texture *texture = nullptr;

static uint32_t tile_color[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

uint32_t get_ticks(){
  return SDL_GetTicks64();
}

void delay(uint32_t ms){
  SDL_Delay(ms);
}

void draw_tile(WORD start_address, WORD tile_index, int pos_x, int pos_y){
  for(int y=0; y<16; y+=2){
    BYTE first_row = bus_read(start_address + tile_index * TILE_SIZE + y);
    BYTE second_row = bus_read(start_address + tile_index * TILE_SIZE + y + 1);
    for(int x=7; x>=0; x--){
      BYTE lo = BIT(first_row, x);
      BYTE hi = BIT(second_row, x);
      BYTE color = (hi << 1) | lo;

      SDL_Rect tile_rect;
      tile_rect.x = pos_x + SCALE * (7 - x);
      tile_rect.y = pos_y + SCALE * (y / 2);
      tile_rect.w = SCALE;
      tile_rect.h = SCALE;
      SDL_FillRect(dbg_screen_surface, &tile_rect, tile_color[color]);
    }
  }
}

void update_debug_window(){
  SDL_FillRect(dbg_screen_surface, NULL, 0xFF111111);

  WORD tile_index = 0;
  for(int y=0; y<16; y++){
    for(int x=0; x<24; x++){
      draw_tile(0x8000, tile_index, x * SCALE * TILE_WIDTH + x * SCALE, y * SCALE * TILE_HEIGHT + y * SCALE);
      tile_index++;
    }
  }
  SDL_UpdateTexture(dbg_texture, NULL, dbg_screen_surface->pixels, dbg_screen_surface->pitch);
  SDL_RenderClear(dbg_renderer);
  SDL_RenderCopy(dbg_renderer, dbg_texture, NULL, NULL);
  SDL_RenderPresent(dbg_renderer);
}

void update_window(){
  for(int y=0; y<144; y++){
    for(int x=0; x<160; x++){
      uint32_t color = ppu_get_context()->video_buffer[y][x];
      SDL_Rect tile_rect;
      tile_rect.x = x * SCALE;
      tile_rect.y = y * SCALE;
      tile_rect.w = SCALE;
      tile_rect.h = SCALE;
      SDL_FillRect(screen_surface, &tile_rect, color);
    }
  }
  SDL_UpdateTexture(texture, NULL, screen_surface->pixels, screen_surface->pitch);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void ui_init(){
  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();

  SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
  SDL_CreateWindowAndRenderer(DEBUG_SCREEN_WIDTH, DEBUG_SCREEN_HEIGHT, 0, &dbg_window, &dbg_renderer);

  screen_surface = SDL_CreateRGBSurface(0,
                                       SCREEN_WIDTH,
                                       SCREEN_HEIGHT,
                                       32,
                                       0x00FF0000,
                                       0x0000FF00,
                                       0x000000FF,
                                       0xFF000000);
  dbg_screen_surface = SDL_CreateRGBSurface(0,
                                       DEBUG_SCREEN_WIDTH,
                                       DEBUG_SCREEN_HEIGHT,
                                       32,
                                       0x00FF0000,
                                       0x0000FF00,
                                       0x000000FF,
                                       0xFF000000);


  dbg_texture = SDL_CreateTexture(dbg_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, DEBUG_SCREEN_WIDTH, DEBUG_SCREEN_HEIGHT);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void ui_handle_events(){
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) emu_get_context()->quit = true;
  }
}

void ui_update(){
  update_window();
  update_debug_window();
}

void ui_quit(){
  SDL_DestroyWindow(dbg_window);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
