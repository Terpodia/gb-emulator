#include "emu.h"
#include <SDL3/SDL_main.h>

int main(int argc, char **argv) {
  int a = 2;
  char *b[] = {argv[0], "Legend of Zelda, The - Link's Awakening (USA, Europe) (Rev 2).gb", NULL};
  return emu_run(a, b); 
}
