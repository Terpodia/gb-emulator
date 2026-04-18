#include "cpu.h"
#include "bus.h"
#include "emu.h"

extern cpu_context ctx;

bool fetch_data() {
  if (ctx.current_instruction == nullptr) return false;
  ctx.memory_destination = 0;
  ctx.destination_is_memory = false;

  switch (ctx.current_instruction->addr_mode) {
    case AM_IMP:
      break;

    case AM_R_D16: {
      emu_cycles(1);
      WORD lo = bus_read(ctx.cpu_regs.pc);

      emu_cycles(1);
      WORD hi = bus_read(ctx.cpu_regs.pc + 1);

      ctx.fetched_data = (hi << 8) | lo;
      ctx.cpu_regs.pc += 2;
      break;
    }

    case AM_R_R:
      ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);
      break;

    case AM_MR_R:{
      WORD address = cpu_read_reg(ctx.current_instruction->reg_1);
      if(ctx.current_instruction->reg_1 == RT_C) address += 0xFF00;

      ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);

      ctx.memory_destination = address;
      ctx.destination_is_memory = true;
      break;
    }

    case AM_R:
      ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_1);
      break;

    case AM_R_D8:
      emu_cycles(1);
      ctx.fetched_data = bus_read(ctx.cpu_regs.pc);
      ctx.cpu_regs.pc++;
      break;

    case AM_R_MR:{
      WORD address = cpu_read_reg(ctx.current_instruction->reg_2);
      if(ctx.current_instruction->reg_2 == RT_C) address += 0xFF00;

      emu_cycles(1);
      ctx.fetched_data = bus_read(address);
      break;
    }

    case AM_R_HLI:{
      WORD address = cpu_read_reg(ctx.current_instruction->reg_2);

      emu_cycles(1);
      ctx.fetched_data = bus_read(address);

      WORD val = cpu_read_reg(RT_HL);
      cpu_write_reg(RT_HL, val+1);
      break;
    }

    case AM_R_HLD:{
      WORD address = cpu_read_reg(ctx.current_instruction->reg_2);

      emu_cycles(1);
      ctx.fetched_data = bus_read(address);

      WORD val = cpu_read_reg(RT_HL);
      cpu_write_reg(RT_HL, val-1);
      break;
    }

    case AM_HLI_R:{
      WORD address = cpu_read_reg(ctx.current_instruction->reg_1);

      ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);
      ctx.memory_destination = address;
      ctx.destination_is_memory = true;

      WORD val = cpu_read_reg(RT_HL);
      cpu_write_reg(RT_HL, val+1);
      break;
    }

    case AM_HLD_R:{
      WORD address = cpu_read_reg(ctx.current_instruction->reg_1);

      ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);
      ctx.memory_destination = address;
      ctx.destination_is_memory = true;

      WORD val = cpu_read_reg(RT_HL);
      cpu_write_reg(RT_HL, val-1);
      break;
    }

    case AM_R_A8:{
      emu_cycles(1);
      WORD address = bus_read(ctx.cpu_regs.pc++);

      address |= 0xFF00;

      emu_cycles(1);
      ctx.fetched_data = bus_read(address);

      break;
    }

    case AM_A8_R:{
      emu_cycles(1);
      WORD address = bus_read(ctx.cpu_regs.pc++);

      address |= 0xFF00;

      ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);
      ctx.memory_destination = address;
      ctx.destination_is_memory = true;

      break;
    }

    case AM_HL_SPR:{
      emu_cycles(1);
      ctx.fetched_data = bus_read(ctx.cpu_regs.pc++);
      break;
    }

    case AM_D16: {
      emu_cycles(1);
      WORD lo = bus_read(ctx.cpu_regs.pc);

      emu_cycles(1);
      WORD hi = bus_read(ctx.cpu_regs.pc + 1);

      ctx.fetched_data = (hi << 8) | lo;
      ctx.cpu_regs.pc += 2;
      break;
    }

    case AM_D8: {
      emu_cycles(1);
      ctx.fetched_data = bus_read(ctx.cpu_regs.pc++);
      break;
    }

    case AM_MR_D8: {
      emu_cycles(1);
      ctx.fetched_data = bus_read(ctx.cpu_regs.pc++);
      WORD address = cpu_read_reg(ctx.current_instruction->reg_1);

      ctx.memory_destination = address;
      ctx.destination_is_memory = true;
      break;
    }

    case AM_MR: {
      WORD address = cpu_read_reg(ctx.current_instruction->reg_1);
      ctx.memory_destination = address;
      ctx.destination_is_memory = true;
      emu_cycles(1);
      ctx.fetched_data = bus_read(address);
      break;
    }

    case AM_A16_R: {
      emu_cycles(1);
      WORD lo = bus_read(ctx.cpu_regs.pc++);

      emu_cycles(1);
      WORD hi = bus_read(ctx.cpu_regs.pc++);

      WORD address = (hi << 8) | lo;

      ctx.memory_destination = address;
      ctx.destination_is_memory = true;

      ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);
      break;
    }

    case AM_R_A16: {
      emu_cycles(1);
      WORD lo = bus_read(ctx.cpu_regs.pc++);

      emu_cycles(1);
      WORD hi = bus_read(ctx.cpu_regs.pc++);

      WORD address = (hi << 8) | lo;

      emu_cycles(1);
      ctx.fetched_data = bus_read(address);

      break;
    }
    
    default:
      std::cout << "Unknown Addressing Mode: "
                << ctx.current_instruction->addr_mode << std::hex << "("
                << ctx.cur_opcode << ")" << std::dec << "\n";
      return false;
  }

  return true;
}
