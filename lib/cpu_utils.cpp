#include <cpu.h>

extern cpu_context ctx;

WORD reverse(WORD n) { return (n >> 8) | ((n & 0x00FF) << 8); }

WORD cpu_read_reg(register_type reg_type) {
  switch (reg_type) {
    case RT_A:
      return ctx.cpu_regs.a;
    case RT_B:
      return ctx.cpu_regs.b;
    case RT_C:
      return ctx.cpu_regs.c;
    case RT_D:
      return ctx.cpu_regs.d;
    case RT_E:
      return ctx.cpu_regs.e;
    case RT_F:
      return ctx.cpu_regs.f;
    case RT_H:
      return ctx.cpu_regs.h;
    case RT_L:
      return ctx.cpu_regs.l;

    case RT_AF:
      return reverse(*((WORD *)&ctx.cpu_regs.a));
    case RT_BC:
      return reverse(*((WORD *)&ctx.cpu_regs.b));
    case RT_DE:
      return reverse(*((WORD *)&ctx.cpu_regs.d));
    case RT_HL:
      return reverse(*((WORD *)&ctx.cpu_regs.h));

    case RT_SP:
      return ctx.cpu_regs.sp;
    case RT_PC:
      return ctx.cpu_regs.pc;

    default:
      return 0;
  }
}
