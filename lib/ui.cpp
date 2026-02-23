#include <ui.h>
#include <emu.h>
#include <bus.h>
#include <ppu.h>
#include <joypad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int SCALE = 6;
const int TILE_WIDTH = 8;
const int TILE_HEIGHT = 8;

const int SCREEN_WIDTH = 160 * SCALE;
const int SCREEN_HEIGHT = 144 * SCALE;

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

  screen_surface = SDL_CreateRGBSurface(0,
                                       SCREEN_WIDTH,
                                       SCREEN_HEIGHT,
                                       32,
                                       0x00FF0000,
                                       0x0000FF00,
                                       0x000000FF,
                                       0xFF000000);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void ui_handle_key(BYTE keycode, bool pressed){
  switch(keycode){
    case SDLK_j: joypad_get_context()->state.a = pressed; break;
    case SDLK_k: joypad_get_context()->state.b = pressed; break;
    case SDLK_w: joypad_get_context()->state.up = pressed; break;
    case SDLK_a: joypad_get_context()->state.left = pressed; break;
    case SDLK_s: joypad_get_context()->state.down = pressed; break;
    case SDLK_d: joypad_get_context()->state.right = pressed; break;
    case SDLK_TAB: joypad_get_context()->state.start = pressed; break;
    case SDLK_RETURN: joypad_get_context()->state.select = pressed; break;
  }
}

void ui_handle_events(){
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
      emu_get_context()->quit = true;

    if(e.type == SDL_KEYDOWN) ui_handle_key(e.key.keysym.sym, true);
    if(e.type == SDL_KEYUP) ui_handle_key(e.key.keysym.sym, false);
  }
}

void ui_update(){
  update_window();
}

void ui_quit(){
  SDL_DestroyWindow(window);
  SDL_Quit();
}
