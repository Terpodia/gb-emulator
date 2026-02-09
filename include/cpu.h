#pragma once

#include <common.h>
#include <instructions.h>

struct cpu_registers {
  BYTE a;
  BYTE f;
  BYTE b;
  BYTE c;
  BYTE d;
  BYTE e;
  BYTE h;
  BYTE l;
  WORD pc;
  WORD sp;
};

struct cpu_context {
  cpu_registers cpu_regs;

  WORD fetched_data;
  WORD memory_destination;
  BYTE cur_opcode;
  instruction *current_instruction;

  BYTE interrupt_enable_register;
  BYTE interrupt_flag;

  bool interrupt_master_enable;
  bool enabling_interrupt_master;
  bool destination_is_memory;
  bool halted;
  bool stepping;
};

#define CPU_FLAG_Z BIT(ctx.cpu_regs.f, 7)
#define CPU_FLAG_N BIT(ctx.cpu_regs.f, 6)
#define CPU_FLAG_H BIT(ctx.cpu_regs.f, 5)
#define CPU_FLAG_C BIT(ctx.cpu_regs.f, 4)

void cpu_init();
bool cpu_step();

bool fetch_data();

typedef void (*IN_PROC)();

void setup_instruction_processor();
IN_PROC get_instruction_processor();

WORD cpu_read_reg(register_type reg_type);
void cpu_write_reg(register_type reg_type, WORD value);

BYTE cpu_read_reg8(register_type reg_type);
void cpu_write_reg8(register_type reg_type, BYTE value);

BYTE cpu_read_interrupt_enable_register();
void cpu_write_interrupt_enable_register(BYTE value);
