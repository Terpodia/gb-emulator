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
        BYTE h = ((value & 0xF) + (ctx.fetched_data & 0xF)) > 0xF;
        BYTE c = ((value & 0xFF) + (ctx.fetched_data & 0xFF)) > 0xFF;

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

void proc_INC(){
    if(ctx.destination_is_memory){
        bus_write(ctx.memory_destination, ctx.fetched_data + 1);
        emu_cycles(1);

        BYTE h = ((ctx.fetched_data & 0xF) + 1) > 0xF;
        BYTE z = (ctx.fetched_data + 1) == 0;
        cpu_set_flags(z, 0, h, -1);

        return;
    }
    cpu_write_reg(ctx.current_instruction->reg_1, ctx.fetched_data + 1);
    if(is_16_bit_register(ctx.current_instruction->reg_1)){
        emu_cycles(1);
        return;
    }
    BYTE h = ((ctx.fetched_data & 0xF) + 1) > 0xF;
    BYTE z = ((ctx.fetched_data + 1) & 0xFF) == 0;
    cpu_set_flags(z, 0, h, -1);
}

void proc_DEC(){
    if(ctx.destination_is_memory){
        bus_write(ctx.memory_destination, ctx.fetched_data - 1);
        emu_cycles(1);

        BYTE h = ((ctx.fetched_data & 0xF) == 0);
        BYTE z = (ctx.fetched_data - 1) == 0;
        cpu_set_flags(z, 1, h, -1);

        return;
    }
    cpu_write_reg(ctx.current_instruction->reg_1, ctx.fetched_data - 1);
    if(is_16_bit_register(ctx.current_instruction->reg_1)){
        emu_cycles(1);
        return;
    }

    BYTE h = ((ctx.fetched_data & 0xF) == 0);
    BYTE z = (ctx.fetched_data - 1) == 0;
    cpu_set_flags(z, 1, h, -1);
}

void proc_ADD(){
    if(ctx.current_instruction->reg_1 == RT_HL){
        WORD value = cpu_read_reg(ctx.current_instruction->reg_1);

        cpu_write_reg(ctx.current_instruction->reg_1, value + ctx.fetched_data);

        BYTE h = ((value & 0xFFF) + (ctx.fetched_data & 0xFFF)) > 0xFFF;
        BYTE c = ((int)value + (int)ctx.fetched_data) > 0xFFFF;
        cpu_set_flags(-1, 0, h, c);

        emu_cycles(1);
        return;
    }
    if(ctx.current_instruction->reg_1 == RT_SP){
        WORD value = cpu_read_reg(ctx.current_instruction->reg_1);

        cpu_write_reg(ctx.current_instruction->reg_1, value + (char)ctx.fetched_data);

        BYTE h = ((value & 0xF) + (ctx.fetched_data) & 0xF) > 0xF;
        BYTE c = ((value & 0xFF) + (ctx.fetched_data) & 0xFF) > 0xFF;
        cpu_set_flags(0, 0, h, c);

        emu_cycles(2);
        return;
    }
    WORD value = cpu_read_reg(ctx.current_instruction->reg_1);

    cpu_write_reg(ctx.current_instruction->reg_1, value + ctx.fetched_data);

    BYTE z = ((value + ctx.fetched_data) & 0xFF) == 0;
    BYTE h = ((value & 0xF) + (ctx.fetched_data & 0xF)) > 0xF;
    BYTE c = ((value & 0xFF) + (ctx.fetched_data & 0xFF)) > 0xFF;
    cpu_set_flags(z, 0, h, c);
}

void proc_SUB(){
  WORD value = cpu_read_reg(ctx.current_instruction->reg_1);
  cpu_write_reg(ctx.current_instruction->reg_1, value - ctx.fetched_data);

  BYTE z = ((value - ctx.fetched_data) & 0xFF) == 0;
  BYTE h = (value & 0xF) < (ctx.fetched_data & 0xF);
  BYTE c = (value & 0xFF) < (ctx.fetched_data & 0xFF);

  cpu_set_flags(z, 1, h, c);
}

void proc_ADC(){
  WORD value = cpu_read_reg(ctx.current_instruction->reg_1);
  cpu_write_reg(ctx.current_instruction->reg_1, value + ctx.fetched_data + CPU_FLAG_C);
  BYTE z = ((value + ctx.fetched_data + CPU_FLAG_C) & 0xFF) == 0;
  BYTE h = ((value & 0xF) + (ctx.fetched_data & 0xF) + CPU_FLAG_C) > 0xF;
  BYTE c = ((value & 0xFF) + (ctx.fetched_data & 0xFF) + CPU_FLAG_C) > 0xFF;
  cpu_set_flags(z, 0, h, c);
}

void proc_SBC(){
  WORD value = cpu_read_reg(ctx.current_instruction->reg_1);
  cpu_write_reg(ctx.current_instruction->reg_1, value - ctx.fetched_data - CPU_FLAG_C);

  BYTE z = ((value - ctx.fetched_data - CPU_FLAG_C) & 0xFF) == 0;
  BYTE h = (value & 0xF) < ((ctx.fetched_data & 0xF) + CPU_FLAG_C);
  BYTE c = (value & 0xFF) < ((ctx.fetched_data & 0xFF) + CPU_FLAG_C);

  cpu_set_flags(z, 1, h, c);
}

void proc_CP(){
  WORD value = cpu_read_reg(ctx.current_instruction->reg_1);
  BYTE z = ((value - ctx.fetched_data) & 0xFF) == 0;
  BYTE h = (value & 0xF) < (ctx.fetched_data & 0xF);
  BYTE c = (value & 0xFF) < (ctx.fetched_data & 0xFF);
  cpu_set_flags(z, 1, h, c);
}

void proc_AND(){
  WORD value = cpu_read_reg(ctx.current_instruction->reg_1);
  cpu_write_reg(ctx.current_instruction->reg_1, value & ctx.fetched_data);

  BYTE z = (value & ctx.fetched_data) == 0;

  cpu_set_flags(z, 0, 1, 0);
}

void proc_OR(){
  WORD value = cpu_read_reg(ctx.current_instruction->reg_1);
  cpu_write_reg(ctx.current_instruction->reg_1, value | ctx.fetched_data);

  BYTE z = (value | ctx.fetched_data) == 0;

  cpu_set_flags(z, 0, 0, 0);
}

void proc_XOR(){
  WORD value = cpu_read_reg(ctx.current_instruction->reg_1);
  cpu_write_reg(ctx.current_instruction->reg_1, value ^ ctx.fetched_data);

  BYTE z = (value ^ ctx.fetched_data) == 0;

  cpu_set_flags(z, 0, 0, 0);
}

register_type rt_lookup[] = {RT_B, RT_C, RT_D, RT_E, RT_H, RT_L, RT_HL, RT_A};

void proc_CB(){
  WORD op_code = ctx.fetched_data;
  register_type reg = rt_lookup[op_code & 7];

  if(reg == RT_HL) emu_cycles(2);

  BYTE val = cpu_read_reg8(reg);
  BYTE bit_index = (op_code >> 3) & 7;

  switch((op_code >> 6) & 3){
    case 1:
      cpu_set_flags(!BIT(val, bit_index), 0, 1, -1);
      return;
   
    case 2:
      val &= ~(1<<bit_index);
      cpu_write_reg8(reg, val);
      return;
    
    case 3:
      val |= (1<<bit_index);
      cpu_write_reg8(reg, val);
      return;
  }
  switch((op_code >> 3) & 7){
    case 0:{
      BYTE c = BIT(val, 7);
      val <<= 1;
      if(c) val |= 1;

      BYTE z = val == 0;

      cpu_write_reg8(reg, val);
      cpu_set_flags(z, 0, 0, c);
      return;
    }
    case 1:{
      BYTE c = val & 1;
      val >>= 1;
      if(c) val |= (1<<7);

      BYTE z = val == 0;

      cpu_write_reg8(reg, val);
      cpu_set_flags(z, 0, 0, c);
      return;
    }
    case 2:{
      BYTE c = BIT(val, 7);
      val <<= 1;
      val |= CPU_FLAG_C;

      BYTE z = val == 0;

      cpu_write_reg8(reg, val);
      cpu_set_flags(z, 0, 0, c);
      return;
    }

    case 3:{
      BYTE c = val & 1;
      val >>= 1;
      if(CPU_FLAG_C) val |= (1<<7);

      BYTE z = val == 0;

      cpu_write_reg8(reg, val);
      cpu_set_flags(z, 0, 0, c);
      return;
    }

    case 4:{
      BYTE c = BIT(val, 7);
      val <<= 1;

      BYTE z = val == 0;

      cpu_write_reg8(reg, val);
      cpu_set_flags(z, 0, 0, c);
      return;
    }

    case 5:{
      BYTE c = val & 1;
      val = (int8_t)val >> 1;

      BYTE z = val == 0;

      cpu_write_reg8(reg, val);
      cpu_set_flags(z, 0, 0, c);
      return;
    }

    case 6:{
      val = (val >> 4) | (val << 4);
      cpu_write_reg8(reg, val);
      cpu_set_flags(val == 0, 0, 0, 0);
      return;
    }

    case 7:{
      BYTE c = val & 1;
      val >>= 1;

      BYTE z = val == 0;

      cpu_write_reg8(reg, val);
      cpu_set_flags(z, 0, 0, c);
      return;
    }
  }
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
  instruction_processor[IN_INC] = proc_INC;
  instruction_processor[IN_DEC] = proc_DEC;
  instruction_processor[IN_ADD] = proc_ADD;
  instruction_processor[IN_SUB] = proc_SUB;
  instruction_processor[IN_ADC] = proc_ADC;
  instruction_processor[IN_SBC] = proc_SBC;
  instruction_processor[IN_CP] = proc_CP;
  instruction_processor[IN_AND] = proc_AND;
  instruction_processor[IN_OR] = proc_OR;
  instruction_processor[IN_XOR] = proc_XOR;
  instruction_processor[IN_CB] = proc_CB;
}

IN_PROC get_instruction_processor(){
  return instruction_processor[ctx.current_instruction->in_type];
}
