#include "io.h"
#include "joypad.h"
#include "ram.h"
#include "timer.h"
#include "serial.h"
#include "cpu.h"
#include "dma.h"
#include "hdma.h"
#include "lcd.h"
#include "apu.h"
#include "ppu.h"

BYTE io_read(WORD address){
  if(address == 0xFF00) return joypad_read();

  if(address >= 0xFF01 && address <= 0xFF02) return serial_read(address);

  if(address >= 0xFF04 && address <= 0xFF07) return timer_read(address);

  if(address == 0xFF0F) return cpu_read_interrupt_flag();

  if(address >= 0xFF10 && address <= 0xFF3F) return apu_read(address);

  if(address >= 0xFF40 && address <= 0xFF4B) return lcd_read(address);

  if(address >= 0xFF68 && address <= 0xFF6B) return lcd_cgb_read(address);

  if(address == 0xFF4F) return ppu_get_vram_bank();

  if(address == 0xFF70) return get_wram_bank();

  if(address == 0xFF55) return hdma_remaining_length();

  if(address == 0xFF4D) return speed_mode();

  std::cout << "I/O Register Not suported yet\n";
  std::cout << "Accessing: " << std::hex << address << std::dec << "\n";

  return 0;
}
void io_write(WORD address, BYTE value){
  if(address == 0xFF00){
    joypad_write(value);
    return;
  }
  if(address >= 0xFF01 && address <= 0xFF02){
    serial_write(address, value);
    return;
  }
  if(address >= 0xFF04 && address <= 0xFF07){
    timer_write(address, value);
    return;
  }
  if(address == 0xFF0F){
    cpu_write_interrupt_flag(value);
    return;
  }
  if(address >= 0xFF10 && address <= 0xFF3F){
    apu_write(address, value);
    return;
  }
  if(address >= 0xFF40 && address <= 0xFF4B){
    lcd_write(address, value);
    return;
  }
  if(address >= 0xFF68 && address <= 0xFF6B){
    lcd_cgb_write(address, value);
    return;
  }
  if(address == 0xFF4F){
    ppu_set_vram_bank(value);
    return;
  }
  if(address == 0xFF70){
    set_wram_bank(value);
    return;
  }
  if(address >= 0xFF51 && address <= 0xFF52){
    hdma_load_source(address, value);
    return;
  }
  if(address >= 0xFF53 && address <= 0xFF54){
    hdma_load_destination(address, value);
    return;
  }
  if(address == 0xFF55){
    hdma_trigger_transfer(value);
    return;
  }
  if(address == 0xFF4D){
    prepare_speed_switch(value);
    return;
  }

  std::cout << "I/O Register Not suported yet\n";
  std::cout << "Accessing: " << std::hex << address << std::dec << "\n";
  std::cout << "Value:" << (int)value << "\n";
}
