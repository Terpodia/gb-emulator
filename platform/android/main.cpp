#include "emu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_dialog.h>
#include <string.h>

static const SDL_DialogFileFilter filters[] = {
  {"GB ROMs", "gb"}
};

bool browsing = true;
char selected_rom[1024];

static void SDLCALL OnFileSelected(void *userdata, const char* const* filelist, int filter){
  browsing = false;
  if(filelist && *filelist){
    for(int i = 0; i < strlen(*filelist); i++)
      selected_rom[i] = (*filelist)[i];
  }
}

void browse_rom(){
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_HAPTIC);

  SDL_Window *window = NULL;
  window = SDL_CreateWindow("browser", 500, 500, 0);

  SDL_ShowOpenFileDialog(
      OnFileSelected,   // callback
      nullptr,          // userdata
      window,           // ventana modal
      filters,          // filtros
      SDL_arraysize(filters),
      nullptr,          // carpeta inicial (o una ruta)
      false             // true = permitir varios archivos
  );

  while(browsing){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
      if(e.type == SDL_EVENT_QUIT) browsing = false;
    }
  }

  SDL_DestroyWindow(window);
}

int main(int argc, char **argv) {
  browse_rom();
  if(!selected_rom[0]){
    SDL_Quit();
    return 0;
  }
  char *arg[]  { argv[0], selected_rom };
  return emu_run(2, arg); 
}

