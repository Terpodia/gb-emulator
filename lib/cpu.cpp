#include <cpu.h>
#include <bus.h>
#include <emu.h>

cpu_context ctx;

void cpu_init(){
    setup_instructions_types();
    ctx.cpu_regs.pc = 0x100;
}

static void fetch_instruction(){
    ctx.cur_opcode = bus_read(ctx.cpu_regs.pc++);
    ctx.current_instruction = instruction_by_opcode(ctx.cur_opcode);
    if(ctx.current_instruction->in_type == IN_NONE){ 
        std::cout << "Unknown Instruction: " << std::hex << "(" << (int)ctx.cur_opcode << ")" 
                  << std::dec << "\n";
        ctx.current_instruction = nullptr;
    }
}

static bool fetch_data(){
    if(ctx.current_instruction == nullptr) return false;
    ctx.memory_destination = 0;
    ctx.destination_is_memory = false;

    switch (ctx.current_instruction->addr_mode){
        case AM_IMP:
            break;

        case AM_R:
            ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_1);
            break;
        
        case AM_R_D8:
            ctx.fetched_data = bus_read(ctx.cpu_regs.pc);
            emu_cycles(1);
            ctx.cpu_regs.pc++;
            break;

        case AM_D16: {
            WORD lo = bus_read(ctx.cpu_regs.pc);
            emu_cycles(1);

            WORD hi = bus_read(ctx.cpu_regs.pc+1);
            emu_cycles(1);

            ctx.fetched_data = (hi << 8) | lo;
            ctx.cpu_regs.pc += 2;
            break;
        }

        default:
            std::cout << "Unknown Addressing Mode: " << ctx.current_instruction->addr_mode 
                      << std::hex << "(" << ctx.cur_opcode << ")" << std::dec << "\n";
            return false;
    }

    return true;
}

static void execute(){
    std::cout << "Not executing yet\n";
}

bool cpu_step(){
    if(!ctx.halted){
        WORD pc = ctx.cpu_regs.pc;

        fetch_instruction();
        if(!fetch_data()) return false;

        std::cout << "Executing CPU step: " << std::hex << pc << " " <<  std::dec
                  << ctx.current_instruction->in_type << std::hex 
                  << " (" << (int)ctx.cur_opcode << ")" << std::dec << "\n";

        execute();
    }
    return true;
}