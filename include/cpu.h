#pragma once

#include <common.h>
#include <instructions.h>

struct cpu_registers{
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

struct cpu_context{
    cpu_registers cpu_regs;
    
    WORD fetched_data;
    WORD memory_destination;
    BYTE cur_opcode;
    instruction *current_instruction;

    bool destination_is_memory;
    bool halted;
    bool stepping;
};

void cpu_init();

bool cpu_step();

WORD cpu_read_reg(register_type reg_type);