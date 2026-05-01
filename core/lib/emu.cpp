#include <cartridge/save_data.h>
#include <cartridge/cart.h>
#include <platform.h>
#include <serial.h>
#include <timer.h>
#include <cpu/cpu.h>
#include <emu.h>
#include <ppu/hdma.h>
#include <ppu/dma.h>
#include <ppu/ppu.h>
#include <apu/apu.h>

static emu_context emu_ctx;

extern cpu_context ctx;

bool odd_cycle;

void emu_cycles(int cpu_cycles) {
  for(int i = 0; i < cpu_cycles; i++){
    for(int j=0; j<4; j++){
      timer_tick();
      serial_tick();
    }
    dma_tick();

    if(ctx.speed_mode == DOUBLE_SPEED_MODE){
      odd_cycle = !odd_cycle;
      if(odd_cycle) continue;
    }
    hdma_tick();
    for(int j=0; j<4; j++){
      ppu_tick();
      apu_tick();
    }
  }
}

emu_context *emu_get_context() {
  return &emu_ctx;
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

  platform_init();
  cpu_init();
  uint64_t frame = 0;
  while (!emu_ctx.quit) {
    //cpu_log();
    if(!cpu_step()){
      std::cout << "CPU Stopped\n";
      return 0;
    }
    if(ppu_get_context()->current_frame != frame){
      frame = ppu_get_context()->current_frame;
      platform_render_update();
      platform_poll_events();
      if(cart_get_context()->should_save_battery) save_battery();
    }
  }
  platform_quit();
  return 0;
}

