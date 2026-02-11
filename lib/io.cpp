#include <io.h>
#include <timer.h>
#include <cpu.h>

BYTE serial[2];

BYTE io_read(WORD address){
  if(address == 0xFF01) return serial[0];

  if(address == 0xFF02) return serial[1];

  if(address >= 0xFF04 && address <= 0xFF07) return timer_read(address);

  if(address == 0xFF0F) return cpu_read_interrupt_flag();

  std::cout << "I/O Register Not suported yet\n";
  std::cout << "Accessing: " << std::hex << address << std::dec << "\n";

  return 0;
}
void io_write(WORD address, BYTE value){
  if(address == 0xFF01){
    serial[0] = value;
    return;
  }
  if(address == 0xFF02){
    serial[1] = value;
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

  std::cout << "I/O Register Not suported yet\n";
  std::cout << "Accessing: " << std::hex << address << std::dec << "\n";
}
