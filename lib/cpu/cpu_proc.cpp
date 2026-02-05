#include <cpu.h>
#include <emu.h>
#include <bus.h>
#include <cpu_stack.h>
#include <instructions.h>

extern cpu_context ctx;

IN_PROC instruction_processor[49];

void cpu_set_flags(BYTE z, BYTE n, BYTE h, BYTE c){
    if(z != -1) BIT_SET(ctx.cpu_regs.f, 7, z);
    if(n != -1) BIT_SET(ctx.cpu_regs.f, 6, n);
    if(h != -1) BIT_SET(ctx.cpu_regs.f, 5, h);
    if(c != -1) BIT_SET(ctx.cpu_regs.f, 4, c);
}

void proc_NOP(){}

bool check_cond(){
  switch(ctx.current_instruction->cond_type){
    case CT_NONE: return true;
    case CT_NZ: return !CPU_FLAG_Z;
    case CT_Z: return CPU_FLAG_Z;
    case CT_NC: return !CPU_FLAG_C;
    case CT_C: return CPU_FLAG_C;
  }
  std:: cout << "ERROR: UNKONWN CONDITION\n";
  return false;
}

void proc_JP(){
  if(check_cond()){
    ctx.cpu_regs.pc = ctx.fetched_data;
    emu_cycles(1);
  }
}

bool is_16_bit_register(register_type rt){
    return rt == RT_AF || rt == RT_BC || rt == RT_DE || rt == RT_HL || rt == RT_SP || rt == RT_PC;
}

void proc_LD(){
    if(ctx.destination_is_memory){
        if(is_16_bit_register(ctx.current_instruction->reg_2)){
            bus_write16(ctx.memory_destination, ctx.fetched_data);
            emu_cycles(1);
        }
        else bus_write(ctx.memory_destination, ctx.fetched_data);
        emu_cycles(1);
        return;
    }
    if(ctx.current_instruction->addr_mode == AM_HL_SPR){
        WORD value = cpu_read_reg(RT_SP);
        BYTE h = (value & (1<<3)) && !((value + ctx.fetched_data) & (1<<3));
        BYTE c = (value & (1<<7)) && !((value + ctx.fetched_data) & (1<<7));

        cpu_write_reg(RT_HL, value + (char)ctx.fetched_data);
        cpu_set_flags(0, 0, h, c);
        return;
    }
    cpu_write_reg(ctx.current_instruction->reg_1, ctx.fetched_data);
}

void proc_LDH(){
    if(ctx.destination_is_memory){
        bus_write(ctx.memory_destination | 0xFF00, ctx.fetched_data);
        emu_cycles(1);
        return;
    }
    cpu_write_reg(ctx.current_instruction->reg_1, ctx.fetched_data);
}

void proc_PUSH(){
    WORD value = cpu_read_reg(ctx.current_instruction->reg_1);
    push((value >> 8) & 0xFF);
    emu_cycles(1);
    push(value & 0xFF);
    emu_cycles(1);

    emu_cycles(1);
}

void proc_POP(){
    WORD lo = pop();
    emu_cycles(1);
    WORD hi = pop();
    emu_cycles(1);

    WORD value = (hi << 8) | lo;

    if(ctx.current_instruction->reg_1 == RT_AF)
        cpu_write_reg(ctx.current_instruction->reg_1, value & 0xFFF0);

    else cpu_write_reg(ctx.current_instruction->reg_1, value);
}

void proc_RET(){
  if(ctx.current_instruction->cond_type != CT_NONE)
      emu_cycles(1);

  if(check_cond()){
      WORD lo = pop();
      emu_cycles(1);
      WORD hi = pop();
      emu_cycles(1);

      WORD pc = (hi << 8) | lo;

      ctx.cpu_regs.pc = pc;

      emu_cycles(1);
  }
}

void proc_CALL(){
    if(check_cond()){
        push((ctx.cpu_regs.pc >> 8) & 0xFF);
        emu_cycles(1);

        push(ctx.cpu_regs.pc & 0xFF);
        emu_cycles(1);

        ctx.cpu_regs.pc = ctx.fetched_data;
        emu_cycles(1);
    }
}

void proc_JR(){
  if(check_cond()){
    ctx.cpu_regs.pc += (char)ctx.fetched_data;
    emu_cycles(1);
  }
}

void proc_RST(){
    push((ctx.cpu_regs.pc >> 8) & 0xFF);
    emu_cycles(1);

    push(ctx.cpu_regs.pc & 0xFF);
    emu_cycles(1);

    ctx.cpu_regs.pc = ctx.current_instruction->param;
    emu_cycles(1);
}

void proc_RETI(){
    ctx.interrupt_master_enable = true;
    proc_RET();
}

void setup_instruction_processor(){
  instruction_processor[IN_NOP] = proc_NOP;
  instruction_processor[IN_JP] = proc_JP;
  instruction_processor[IN_LD] = proc_LD;
  instruction_processor[IN_LDH] = proc_LDH;
  instruction_processor[IN_PUSH] = proc_PUSH;
  instruction_processor[IN_POP] = proc_POP;
  instruction_processor[IN_RET] = proc_RET;
  instruction_processor[IN_CALL] = proc_CALL;
  instruction_processor[IN_JR] = proc_JR;
  instruction_processor[IN_RST] = proc_RST;
  instruction_processor[IN_RETI] = proc_RETI;
}

IN_PROC get_instruction_processor(){
  return instruction_processor[ctx.current_instruction->in_type];
}
