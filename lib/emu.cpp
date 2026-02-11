#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cartridge/cart.h>
#include <cpu.h>
#include <emu.h>
#include <timer.h>

static emu_context ctx;

void delay(unsigned int ms) { SDL_Delay(ms); }

void emu_cycles(int cpu_cycles) {
  for(int i=0; i<cpu_cycles; i++){
    for(int j=0; j<4; j++){
      ctx.ticks++;
      timer_tick();
    }
  }
}

int emu_run(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Usage: ./gbemu <rom_file>\n";
    return -1;
  }

  if (!cart_load(argv[1])) {
    std::cout << "Failed to load ROM file: " << argv[1] << "\n";
    return -2;
  }

  cpu_init();

  SDL_Window *window = nullptr;
  SDL_Surface *screenSurface = nullptr;
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  window =
      SDL_CreateWindow("Gameboy Emulator", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL,
               SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
  SDL_UpdateWindowSurface(window);
  SDL_Event e;
  bool quit = false;
  while (quit == false) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) quit = true;
    }
    if (!cpu_step()) quit = true;
  }
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
