#include <emu.h>
#include <SDL2/SDL.h>

void delay(unsigned int ms) {
  SDL_Delay(ms);
}

void hello_world() { 
  SDL_Window* window = nullptr;
  SDL_Surface* screenSurface = nullptr;
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
  SDL_UpdateWindowSurface(window);
  SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }
  SDL_DestroyWindow(window);
  SDL_Quit();

  puts("Hello World"); 
}
