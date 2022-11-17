#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <cart.h>
#include <emu.h>

void delay(unsigned int ms) {
  SDL_Delay(ms);
}

int emu_run(int argc, char **argv) { 
  if(argc < 2) {
    std::cout << "Usage: ./gbemu <rom_file>\n";
    return -1;
  }

  if(!cart_load(argv[1])) {
    std::cout << "Failed to load ROM file: " << argv[1] << "\n";
    return -2;
  }

  SDL_Window* window = nullptr;
  SDL_Surface* screenSurface = nullptr;
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
  SDL_UpdateWindowSurface(window);
  SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
