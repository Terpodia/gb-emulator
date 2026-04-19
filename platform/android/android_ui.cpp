#include <SDL_image.h>
#include <SDL.h>
#include <SDL_log.h>
#include "platform.h"
#include "joypad.h"
#include "emu.h"
#include "bus.h"
#include "ppu.h"
#include "apu.h"
#include <tuple>

enum BUTTONS { KEY_A, KEY_B, KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT, KEY_START, KEY_SELECT, KEY_NONE };

struct buttons {
  SDL_Rect key_region[8];
  SDL_Texture *textures[8];

  buttons(){
    untrigger();
  }
  void load_buttons_textures(SDL_Renderer *renderer){
    textures[0] = IMG_LoadTexture(renderer, "button_a.png");
    textures[1] = IMG_LoadTexture(renderer, "button_b.png");
    textures[2] = IMG_LoadTexture(renderer, "dpad_up.png");
    textures[3] = IMG_LoadTexture(renderer, "dpad_down.png");
    textures[4] = IMG_LoadTexture(renderer, "dpad_right.png");
    textures[5] = IMG_LoadTexture(renderer, "dpad_left.png");
    textures[6] = IMG_LoadTexture(renderer, "start.png");
    textures[7] = IMG_LoadTexture(renderer, "select.png");
  }
  SDL_Rect get_key_region(BUTTONS key){
    return key_region[key];
  }
  void set_key_region(BUTTONS key, SDL_Rect r){
    key_region[key] = r;
  }
  bool inside(SDL_Rect r, float x, float y){
    return x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h;
  }
  void untrigger(){
    for(int i = 0; i < 8; i++) SDL_SetTextureColorMod(textures[i], 255, 255, 255);
  }
  void trigger(int i){
    SDL_SetTextureColorMod(textures[i], 180, 180, 180);
  }
  int button_triggered(float x, float y){
    for(int i = 0; i < 8; i++) if(inside(key_region[i], x, y)){
      trigger(i);
      return i;
    }
    return KEY_NONE;
  }
  void draw(SDL_Renderer *renderer){
    for(int i = 0; i < 8; i++)
      SDL_RenderCopy(renderer, textures[i], NULL, &key_region[i]);
  }
};

const int TILE_WIDTH = 8;
const int TILE_HEIGHT = 8;

int screen_scale;
int screen_width;
int screen_height;
int buttons_size;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *texture = nullptr;
SDL_Rect game_viewport;
SDL_Haptic *vibrator;

SDL_Texture *t = nullptr;

buttons virtual_pad;
bool button_was_pressed[8];

static uint32_t tile_color[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

uint32_t get_ticks(){
  return SDL_GetTicks64();
}

void delay(uint32_t ms){
  SDL_Delay(ms);
}

void draw_joypad_buttons(){
  int pad_height = screen_height - game_viewport.y - game_viewport.h;
  buttons_size = screen_width / 7;
  SDL_Rect r;

  r.x = screen_width - screen_width / 30 - buttons_size;
  r.y = screen_height - (pad_height * 60) / 100 - buttons_size;
  r.w = r.h = buttons_size;
  virtual_pad.set_key_region(KEY_A, r);

  r.x = virtual_pad.get_key_region(KEY_A).x - buttons_size;
  r.y = screen_height - (pad_height * 30) / 100 - buttons_size;
  r.w = r.h = buttons_size;
  virtual_pad.set_key_region(KEY_B, r);

  r.x = screen_width / 30 + buttons_size;
  r.y = screen_height - (pad_height * 60) / 100 - buttons_size;
  r.w = r.h = buttons_size;
  virtual_pad.set_key_region(KEY_UP, r);

  r.x = screen_width / 30 + buttons_size;
  r.y = virtual_pad.get_key_region(KEY_UP).y + buttons_size * 2;
  r.w = r.h = buttons_size;
  virtual_pad.set_key_region(KEY_DOWN, r);

  r.x = screen_width / 30;
  r.y = virtual_pad.get_key_region(KEY_UP).y + buttons_size;
  r.w = r.h = buttons_size;
  virtual_pad.set_key_region(KEY_LEFT, r);

  r.x = screen_width / 30 + buttons_size * 2;
  r.y = virtual_pad.get_key_region(KEY_UP).y + buttons_size;
  r.w = r.h = buttons_size;
  virtual_pad.set_key_region(KEY_RIGHT, r);

  pad_height = game_viewport.y;

  r.x = (screen_width - buttons_size * 2) / 2 - buttons_size * 2;
  r.y = (pad_height - buttons_size / 2) / 2;
  r.w = buttons_size * 2;
  r.h = buttons_size / 2;
  virtual_pad.set_key_region(KEY_SELECT, r);

  r.x = (screen_width - buttons_size * 2) / 2 + buttons_size * 2;
  r.y = (pad_height - buttons_size / 2) / 2;
  r.w = buttons_size * 2;
  r.h = buttons_size / 2;
  virtual_pad.set_key_region(KEY_START, r);

  virtual_pad.draw(renderer);
}

void platform_render_update(){
  SDL_UpdateTexture(texture, NULL, ppu_get_context()->video_buffer, XRES * sizeof(uint32_t));

  SDL_GetWindowSize(window, &screen_width, &screen_height);
  screen_scale = screen_width / XRES;

  game_viewport.x = (screen_width - XRES * screen_scale) / 2;
  game_viewport.y = (screen_height - YRES * screen_scale) / 2;
  game_viewport.w = XRES * screen_scale;
  game_viewport.h = YRES * screen_scale;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, &game_viewport);

  draw_joypad_buttons();

  SDL_RenderPresent(renderer);
}

void platform_init(){
  SDL_Init(SDL_INIT_EVERYTHING);
  //TTF_Init();

  SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait");
  SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN, &window, &renderer);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, XRES, YRES);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  if(SDL_InitSubSystem(SDL_INIT_HAPTIC) >= 0){
    vibrator = SDL_HapticOpen(0);
    if(vibrator){
      if(SDL_HapticRumbleInit(vibrator) != 0){
          SDL_HapticClose(vibrator);
          vibrator = NULL;
      }
    }
  }
  if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
    SDL_Log("IMG_Init error: %s", IMG_GetError());
    SDL_Quit();
    exit(-5);
  }

  virtual_pad.load_buttons_textures(renderer);
}

void handle_android_touch(SDL_Event *e){
  joypad_get_context()->state.a = false;
  joypad_get_context()->state.b = false;
  joypad_get_context()->state.up = false;
  joypad_get_context()->state.left = false;
  joypad_get_context()->state.down = false;
  joypad_get_context()->state.right = false;
  joypad_get_context()->state.start = false;
  joypad_get_context()->state.select = false;

  virtual_pad.untrigger();

  bool button_pressed[8];
  for(int i = 0; i < 8; i++) button_pressed[i] = false;

  int touch_id = e->tfinger.touchId;
  int num_fingers = SDL_GetNumTouchFingers(touch_id);
  for(int i = 0; i < num_fingers; i++){
    SDL_Finger *finger = SDL_GetTouchFinger(touch_id, i);
    int button = virtual_pad.button_triggered(finger->x * screen_width, finger->y * screen_height);
    button_pressed[button] = true;
    switch(button){
      case KEY_A: joypad_get_context()->state.a = true; break;
      case KEY_B: joypad_get_context()->state.b = true; break;
      case KEY_UP: joypad_get_context()->state.up = true; break;
      case KEY_LEFT: joypad_get_context()->state.left = true; break;
      case KEY_DOWN: joypad_get_context()->state.down = true; break;
      case KEY_RIGHT: joypad_get_context()->state.right = true; break;
      case KEY_START: joypad_get_context()->state.start = true; break;
      case KEY_SELECT: joypad_get_context()->state.select = true; break;
    }
  }
  bool vibrate = false;
  for(int i = 0; i < 8; i++){
    vibrate |= button_pressed[i] && !button_was_pressed[i];
    button_was_pressed[i] = button_pressed[i];
  }
  if(vibrate) SDL_HapticRumblePlay(vibrator, 0.4, 50);
}

void platform_poll_events(){
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
      emu_get_context()->quit = true;

    if(e.type == SDL_FINGERDOWN || e.type == SDL_FINGERUP || e.type == SDL_FINGERMOTION)
      handle_android_touch(&e);
  }
}

void platform_quit(){
  if(vibrator) SDL_HapticClose(vibrator);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
