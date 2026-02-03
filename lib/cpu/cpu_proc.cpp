#include <cpu.h>
#include <emu.h>
#include <instructions.h>

extern cpu_context ctx;

IN_PROC instruction_processor[49];

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

void setup_instruction_processor(){
  instruction_processor[IN_NOP] = proc_NOP;
  instruction_processor[IN_JP] = proc_JP;
}

IN_PROC get_instruction_processor(){
  return instruction_processor[ctx.current_instruction->in_type];
}
