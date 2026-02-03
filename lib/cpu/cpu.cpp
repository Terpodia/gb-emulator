#include <bus.h>
#include <cpu.h>
#include <emu.h>

cpu_context ctx;

void cpu_init() {
  setup_instructions_types();
  setup_instruction_processor();
  ctx.cpu_regs.pc = 0x100;
  *((WORD *)&ctx.cpu_regs.a) = 0xB001;
  *((WORD *)&ctx.cpu_regs.b) = 0x1300;
  *((WORD *)&ctx.cpu_regs.d) = 0xD800;
  *((WORD *)&ctx.cpu_regs.h) = 0x4D01;
}

static void fetch_instruction() {
  ctx.cur_opcode = bus_read(ctx.cpu_regs.pc++);
  ctx.current_instruction = instruction_by_opcode(ctx.cur_opcode);
  if (ctx.current_instruction->in_type == IN_NONE) {
    std::cout << "Unknown Instruction: " << std::hex << "("
              << (int)ctx.cur_opcode << ")" << std::dec << "\n";
    ctx.current_instruction = nullptr;
  }
}

static void execute() { 
  IN_PROC proc = get_instruction_processor();
  if(!proc) std::cout << "Not executing yet " << std::hex << (int)ctx.cur_opcode << std::dec << "\n";
  else proc();
}

bool cpu_step() {
  if (!ctx.halted) {
    WORD pc = ctx.cpu_regs.pc;

    fetch_instruction();
    if (!fetch_data()) return false;

    std::cout << "Executing CPU step: " << std::hex << pc << " " << std::dec
              << ctx.current_instruction->in_type << std::hex << " ("
              << (int)ctx.cur_opcode << ")" << std::dec << "\n";

    execute();
  }
  return true;
}
