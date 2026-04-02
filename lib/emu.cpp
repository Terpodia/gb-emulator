#include <cartridge/cart.h>
#include <cpu.h>
#include <emu.h>
#include <ui.h>
#include <timer.h>
#include <serial.h>
#include <dma.h>
#include <ppu.h>
#include <apu.h>

static emu_context ctx;

void emu_cycles(int cpu_cycles) {
  for(int i=0; i<cpu_cycles; i++){
    for(int j=0; j<4; j++){
      ctx.ticks++;
      timer_tick();
      ppu_tick();
      apu_tick();
      serial_tick();
    }
    dma_tick();
  }
}

emu_context *emu_get_context() {
  return &ctx;
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

  initialize_banking();
  if(has_battery()) battery_load();

  ui_init();
  cpu_init();
  uint64_t frame = 0;
  while (!ctx.quit) {
    if(!cpu_step()){
      std::cout << "CPU Stopped\n";
      return 0;
    }
    if(ppu_get_context()->current_frame != frame){
      frame = ppu_get_context()->current_frame;
      ui_update();
      ui_handle_events();
      if(cart_get_context()->should_save_battery) save_battery();
    }
  }
  ui_quit();
  return 0;
}

