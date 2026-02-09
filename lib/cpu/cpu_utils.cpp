#include <cpu.h>
#include <bus.h>

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

void cpu_write_reg(register_type reg_type, WORD value) {
  switch (reg_type) {
    case RT_A:
      ctx.cpu_regs.a = value;
      break;
    case RT_B:
      ctx.cpu_regs.b = value;
      break;
    case RT_C:
      ctx.cpu_regs.c = value;
      break;
    case RT_D:
      ctx.cpu_regs.d = value;
      break;
    case RT_E:
      ctx.cpu_regs.e = value;
      break;
    case RT_F:
      ctx.cpu_regs.f = value;
      break;
    case RT_H:
      ctx.cpu_regs.h = value;
      break;
    case RT_L:
      ctx.cpu_regs.l = value;
      break;

    case RT_AF:
      *((WORD *)&ctx.cpu_regs.a) = reverse(value);
      break;
    case RT_BC:
      *((WORD *)&ctx.cpu_regs.b) = reverse(value);
      break;
    case RT_DE:
      *((WORD *)&ctx.cpu_regs.d) = reverse(value);
      break;
    case RT_HL:
      *((WORD *)&ctx.cpu_regs.h) = reverse(value);
      break;

    case RT_SP:
      ctx.cpu_regs.sp = value;
    case RT_PC:
      ctx.cpu_regs.pc = value;
  }
}

BYTE cpu_read_reg8(register_type reg_type) {
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
    case RT_HL:
      return bus_read(reverse(*((WORD *)&ctx.cpu_regs.h)));

    default:
      return 0;
  }
}

void cpu_write_reg8(register_type reg_type, BYTE value) {
  switch (reg_type) {
    case RT_A:
      ctx.cpu_regs.a = value;
      break;
    case RT_B:
      ctx.cpu_regs.b = value;
      break;
    case RT_C:
      ctx.cpu_regs.c = value;
      break;
    case RT_D:
      ctx.cpu_regs.d = value;
      break;
    case RT_E:
      ctx.cpu_regs.e = value;
      break;
    case RT_F:
      ctx.cpu_regs.f = value;
      break;
    case RT_H:
      ctx.cpu_regs.h = value;
      break;
    case RT_L:
      ctx.cpu_regs.l = value;
      break;
    case RT_HL:
      return bus_write(reverse(*((WORD *)&ctx.cpu_regs.h)), value);
      break;
  }
}
