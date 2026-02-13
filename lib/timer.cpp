#include <timer.h>
#include <interrupts.h>

static timer_context ctx;

void timer_init(){
  ctx.div = 0xABCC;
}

void timer_tick(){
  WORD prev_div = ctx.div;
  ctx.div++;
  bool update_timer = false;
  switch(ctx.tac & 0b11){
    case 0:
      if(BIT(prev_div, 9) && !BIT(ctx.div, 9)) update_timer = true;
      break;
    case 1:
      if(BIT(prev_div, 3) && !BIT(ctx.div, 3)) update_timer = true;
      break;
    case 2:
      if(BIT(prev_div, 5) && !BIT(ctx.div, 5)) update_timer = true;
      break;
    case 3:
      if(BIT(prev_div, 7) && !BIT(ctx.div, 7)) update_timer = true;
      break;
  }
  if(update_timer && (ctx.tac & 0b100)){
    ctx.tima++;
    if(ctx.tima == 0){
      ctx.tima = ctx.tma;
      cpu_request_interrupt(INT_TIMER);
    }
  }
}

BYTE timer_read(WORD address){
  switch(address){
    case 0xFF04:
      return ctx.div >> 8;
    case 0xFF05:
      return ctx.tima;
    case 0xFF06:
      return ctx.tma;
    case 0xFF07:
      return ctx.tac;
  }
  NO_IMPL
  return 0;
}

void timer_write(WORD address, BYTE value){
  switch(address){
    case 0xFF04:
      ctx.div = 0;
      break;
    case 0xFF05:
      ctx.tima = value;
      break;
    case 0xFF06:
      ctx.tma = value;
      break;
    case 0xFF07:
      ctx.tac = value;
      break;
  }
}
