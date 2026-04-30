#include <SDL3/SDL.h>
#include <platform.h>
#include <joypad.h>
#include <emu.h>
#include <bus.h>
#include <ppu/ppu.h>

const int SCALE = 6;
const int TILE_WIDTH = 8;
const int TILE_HEIGHT = 8;

const int SCREEN_WIDTH = 160 * SCALE;
const int SCREEN_HEIGHT = 144 * SCALE;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *texture = nullptr;

static uint32_t tile_color[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

uint32_t get_ticks(){
  return SDL_GetTicks();
}

void delay(uint32_t ms){
  SDL_Delay(ms);
}

void platform_render_update(){
  SDL_UpdateTexture(texture, NULL, ppu_get_context()->video_buffer, XRES * sizeof(uint32_t));
  SDL_RenderClear(renderer);
  SDL_RenderTexture(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void platform_init(){
  if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){ 
    std::cerr << "SDL_Init fail: " << SDL_GetError() << "\n";
    exit(-5);
  }

  SDL_CreateWindowAndRenderer("gbemu", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, XRES, YRES);
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

void ui_handle_key(BYTE keycode, bool pressed){
  switch(keycode){
    case SDLK_J: joypad_get_context()->state.a = pressed; break;
    case SDLK_K: joypad_get_context()->state.b = pressed; break;
    case SDLK_W: joypad_get_context()->state.up = pressed; break;
    case SDLK_A: joypad_get_context()->state.left = pressed; break;
    case SDLK_S: joypad_get_context()->state.down = pressed; break;
    case SDLK_D: joypad_get_context()->state.right = pressed; break;
    case SDLK_SPACE: joypad_get_context()->state.start = pressed; break;
    case SDLK_RETURN: joypad_get_context()->state.select = pressed; break;
  }
}

void platform_poll_events(){
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if(e.type == SDL_EVENT_QUIT) emu_get_context()->quit = true;
    if(e.type == SDL_EVENT_KEY_DOWN) ui_handle_key(e.key.key, true);
    if(e.type == SDL_EVENT_KEY_UP) ui_handle_key(e.key.key, false);
  }
}

void platform_quit(){
  SDL_DestroyWindow(window);
  SDL_Quit();
}
