#include <bus.h>
#include <ram.h>
#include <cpu.h>
#include <io.h>
#include <ppu.h>
#include <dma.h>
#include <cartridge/cart.h>

BYTE bus_read(WORD address) {
  if(address <= 0x7FFF){
    // ROM
    return cart_read(address);
  }
  else if(address <= 0x9FFF){
    // VRAM
    return ppu_vram_read(address);
  }
  else if(address <= 0xBFFF){
    // External RAM (Cartridge RAM)
    return cart_read(address);
  }
  else if(address <= 0xDFFF){
    // WRAM
    return wram_read(address); 
  }
  else if(address <= 0xFDFF){
    // Echo RAM - Use of this area is prohibited
    return 0;
  }
  else if(address <= 0xFE9F){
    // OAM
    if(dma_is_active()) return 0;
    return ppu_oam_read(address);
  }
  else if(address <= 0xFEFF){
    // Not usable
    return 0;
  }
  else if(address <= 0xFF7F){
    // I/O Registers
    return io_read(address);
  }
  else if(address <= 0xFFFE){
    // High RAM (HRAM)
    return hram_read(address); 
  }
  else if(address <= 0xFFFF){
    // Interrupt Enable register
    return cpu_read_interrupt_enable_register();
  }
  NO_IMPL
  return 0;
}

void bus_write(WORD address, BYTE value) {
  if(address <= 0x7FFF){
    // ROM
    cart_write(address, value);
  }
  else if(address <= 0x9FFF){
    // VRAM
    ppu_vram_write(address, value);
  }
  else if(address <= 0xBFFF){
    // External RAM (Cartridge RAM)
    cart_write(address, value);
  }
  else if(address <= 0xDFFF){
    // WRAM
    wram_write(address, value);
  }
  else if(address <= 0xFDFF){
    // Echo RAM - Use of this area is prohibited
  }
  else if(address <= 0xFE9F){
    // OAM
    if(dma_is_active()) return;
    ppu_oam_write(address, value);
  }
  else if(address <= 0xFEFF){
    // Not usable
  }
  else if(address <= 0xFF7F){
    // I/O Registers
    io_write(address, value);
  }
  else if(address <= 0xFFFE){
    // High RAM (HRAM)
    hram_write(address, value); 
  }
  else if(address <= 0xFFFF){
    // Interrupt Enable register
    cpu_write_interrupt_enable_register(value);
  }
  else{
    NO_IMPL
  }
}

void bus_write16(WORD address, WORD value) {
  bus_write(address, value & 0xFF);
  bus_write(address + 1, value >> 8);
}
