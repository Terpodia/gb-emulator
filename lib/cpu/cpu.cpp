#include <bus.h>
#include <cpu.h>
#include <dbg.h>
#include <emu.h>
#include <interrupts.h>
#include <timer.h>
#include <iomanip>

cpu_context ctx;

void cpu_init() {
  setup_instructions_types();
  setup_instruction_processor();
  ctx.cpu_regs.pc = 0x100;
  ctx.cpu_regs.sp = 0xFFFE;
  *((WORD *)&ctx.cpu_regs.a) = 0xB001;
  *((WORD *)&ctx.cpu_regs.b) = 0x1300;
  *((WORD *)&ctx.cpu_regs.d) = 0xD800;
  *((WORD *)&ctx.cpu_regs.h) = 0x4D01;

  ctx.interrupt_master_enable = false;
  ctx.enabling_interrupt_master = false;
  ctx.interrupt_enable_register = 0;
  ctx.interrupt_flag = 0;

  timer_init();
}

static void fetch_instruction() {
  ctx.cur_opcode = bus_read(ctx.cpu_regs.pc++);
  ctx.current_instruction = instruction_by_opcode(ctx.cur_opcode);
  if (ctx.current_instruction->in_type == IN_NONE) {
    std::cout << "Unknown Instruction: " << std::hex << "("
              << (int)ctx.cur_opcode << ")" << std::dec << "\n";
    ctx.current_instruction = nullptr;
    NO_IMPL
  }
}

static void execute() { 
  IN_PROC proc = get_instruction_processor();
  if(!proc){
    std::cout << "Not executing yet " << std::hex << (int)ctx.cur_opcode << std::dec << "\n";
    NO_IMPL
  }
  else proc();
}

void cpu_log(){
  WORD pc = ctx.cpu_regs.pc;
  std::cout << std::setfill('0') << std::hex;
  std::cout << "A:" << std::setw(2) << (int)ctx.cpu_regs.a << " "
            << "F:" << std::setw(2) << (int)ctx.cpu_regs.f << " "
            << "B:" << std::setw(2) << (int)ctx.cpu_regs.b << " "
            << "C:" << std::setw(2) << (int)ctx.cpu_regs.c << " "
            << "D:" << std::setw(2) << (int)ctx.cpu_regs.d << " "
            << "E:" << std::setw(2) << (int)ctx.cpu_regs.e << " "
            << "H:" << std::setw(2) << (int)ctx.cpu_regs.h << " "
            << "L:" << std::setw(2) << (int)ctx.cpu_regs.l << " "
            << "SP:" << std::setw(4) << (int)ctx.cpu_regs.sp << " "
            << "PC:" << std::setw(4) << (int)pc << " "
            << "PCMEM:" << std::setw(2) << (int)bus_read(pc) << ","
            << std::setw(2) << (int)bus_read(pc+1) << ","
            << std::setw(2) << (int)bus_read(pc+2) << ","
            << std::setw(2) << (int)bus_read(pc+3) << "\n";
  std::cout << std::dec; 
  dbg_update();
  dbg_print();
}

bool cpu_step() {
  if (!ctx.halted) {
    cpu_log();
    fetch_instruction();
    emu_cycles(1);
    if (!fetch_data()) return false;
    execute();
  }
  else {
    emu_cycles(1);
    if(ctx.interrupt_flag) ctx.halted = false;
  }
  if(ctx.interrupt_master_enable) cpu_handle_interrupts();

  if(ctx.enabling_interrupt_master){
    ctx.interrupt_master_enable = true;
    ctx.enabling_interrupt_master = false;
  }
  return true;
}
