#include <timer.h>
#include <interrupts.h>

static timer_context ctx;

static bool tima_overflow = false;

static const BYTE tac_bit[4] = {9, 3, 5, 7};

void timer_init(){
  ctx.div = 0xABCC;
}

bool should_update_timer(WORD prev_div){
  return BIT(prev_div, tac_bit[ctx.tac & 0b11]) && !BIT(ctx.div, tac_bit[ctx.tac & 0b11]);
}

void tima_tick(WORD prev_div){
  if(tima_overflow){
    tima_overflow = false;
    ctx.tima = ctx.tma;
    cpu_request_interrupt(INT_TIMER);
  }

  if(should_update_timer(prev_div) && (ctx.tac & 0b100)){
    ctx.tima++;
    if(ctx.tima == 0xFF){
      tima_overflow = true;
      ctx.tima = 0;
    }
  }
}

void timer_tick(){
  WORD prev_div = ctx.div;
  ctx.div++;
  tima_tick(prev_div);
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
    case 0xFF04:{
      WORD prev_div = ctx.div;
      ctx.div = 0;
      tima_tick(prev_div);
      break;
    }
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
