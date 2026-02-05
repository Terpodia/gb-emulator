#include <bus.h>
#include <ram.h>
#include <cartridge/cart.h>

BYTE bus_read(WORD address) {
  if(address <= 0x7FFF){
    // ROM
    return cart_read(address);
  }
  else if(address <= 0x9FFF){
    // VRAM
    std::cout << "VRAM Not supported yet\n";
    return 0; 
  }
  else if(address <= 0xBFFF){
    // External RAM (Cartridge RAM)
    return cart_read(address);
  }
  else if(address <= 0xDFFF){
    // WRAM
    wram_read(address); 
    return 0;
  }
  else if(address <= 0xFDFF){
    // Echo RAM - Use of this area is prohibited
    return 0;
  }
  else if(address <= 0xFE9F){
    // OAM
    std::cout << "OAM Not supported yet\n";
    return 0;
  }
  else if(address <= 0xFEFF){
    // Not usable
    return 0;
  }
  else if(address <= 0xFF7F){
    // I/O Registers
    std::cout << "I/O Register Not suported yet\n";
    return 0;
  }
  else if(address <= 0xFFFE){
    // High RAM (HRAM)
    hram_read(address); 
    return 0;
  }
  else if(address <= 0xFFFF){
    // Interrupt Enable register
    std::cout << "Interrupt Enable Register Not suported yet\n";
    return 0;
  }
  return 0;
}

void bus_write(WORD address, BYTE value) {
  if(address <= 0x7FFF){
    // ROM
    cart_write(address, value);
  }
  else if(address <= 0x9FFF){
    // VRAM
    std::cout << "VRAM Not supported yet\n";
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
    std::cout << "OAM Not supported yet\n";
  }
  else if(address <= 0xFEFF){
    // Not usable
  }
  else if(address <= 0xFF7F){
    // I/O Registers
    std::cout << "I/O Register Not suported yet\n";
  }
  else if(address <= 0xFFFE){
    // High RAM (HRAM)
    hram_write(address, value); 
  }
  else if(address <= 0xFFFF){
    // Interrupt Enable register
    std::cout << "Interrupt Enable Register Not suported yet\n";
  }
}

void bus_write16(WORD address, WORD value) {
  cart_write(address, value & 0xFF);
  cart_write(address + 1, value >> 8);
}
