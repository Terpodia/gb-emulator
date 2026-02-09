#include <cpu_stack.h>
#include <cpu.h>
#include <bus.h>

extern cpu_context ctx;

void push(BYTE value){
    bus_write(ctx.cpu_regs.sp--, value);
}
void push16(WORD value){
    push((value >> 8) & 0xFF);
    push(value & 0xFF);
}

BYTE pop(){
    ctx.cpu_regs.sp++;
    return bus_read(ctx.cpu_regs.sp);
}
WORD pop16(){
    WORD lo = pop();
    WORD hi = pop();
    return (hi << 8) | lo;
}
