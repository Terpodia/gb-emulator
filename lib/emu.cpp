#include <cartridge/cart.h>
#include <cpu.h>
#include <emu.h>
#include <ui.h>
#include <timer.h>
#include <pthread.h>
#include <unistd.h>
#include <dma.h>
#include <ppu.h>

static emu_context ctx;

void emu_cycles(int cpu_cycles) {
  for(int i=0; i<cpu_cycles; i++){
    for(int j=0; j<4; j++){
      ctx.ticks++;
      timer_tick();
      ppu_tick();
    }
    dma_tick();
  }
}

emu_context *emu_get_context() {
  return &ctx;
}

void *cpu_run(void *p){
  cpu_init();
  while (!ctx.quit){
    if(!cpu_step()){
      std::cout << "CPU Stopped\n";
      return 0;
    }
  }
  return 0;
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

  pthread_t cpu_thread;
  if(pthread_create(&cpu_thread, NULL, cpu_run, NULL)){
    std::cout << "Failed to create cpu thread\n";
    return -1;
  }

  ui_init();
  uint64_t frame = 0;
  while (!ctx.quit) {
    usleep(1000);
    if(ppu_get_context()->current_frame != frame){
      frame = ppu_get_context()->current_frame;
      ui_update();
    }
    ui_handle_events();
  }
  ui_quit();
  return 0;
}

