#include <interrupts.h>
#include <cpu.h>
#include <cpu_stack.h>
#include <emu.h>

extern cpu_context ctx;

bool int_check(BYTE interrupt_type, WORD address){
  if(!(ctx.interrupt_enable_register & interrupt_type)) return false;
  if(!(ctx.interrupt_flag & interrupt_type)) return false;

  ctx.interrupt_flag &= ~interrupt_type;
  ctx.interrupt_master_enable = false;
  ctx.halted = false;

  emu_cycles(2);
  push16(ctx.cpu_regs.pc);
  emu_cycles(2);
  ctx.cpu_regs.pc = address;
  emu_cycles(1);

  return true;
}

void cpu_handle_interrupts(){
  if(int_check(INT_VBLANK, 0x40)) return;
  if(int_check(INT_LCD_STAT, 0x48)) return;
  if(int_check(INT_TIMER, 0x50)) return;
  if(int_check(INT_SERIAL, 0x58)) return;
  if(int_check(INT_JOYPAD, 0x60)) return;
}

void cpu_request_interrupt(BYTE interrupt_type){
  ctx.interrupt_flag |= interrupt_type;
}
