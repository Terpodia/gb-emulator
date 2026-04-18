#include "timer.h"
#include "interrupts.h"

static timer_context ctx;

static bool tima_overflow = false;
static int ticks_after_overflow = 0;

static const BYTE tac_bit[4] = {9, 3, 5, 7};

void timer_init(){
  ctx.div = 0;
}

bool should_update_timer(WORD prev_div){
  return BIT(prev_div, tac_bit[ctx.tac & 0b11]) && !BIT(ctx.div, tac_bit[ctx.tac & 0b11]);
}

void tima_tick(WORD prev_div){
  if(tima_overflow){
    ticks_after_overflow++;
    if(ticks_after_overflow == 4) cpu_request_interrupt(INT_TIMER);
    if(ticks_after_overflow == 5) ctx.tima = ctx.tma;
    if(ticks_after_overflow == 6)
      tima_overflow = false, ticks_after_overflow = 0;
  }
  if(should_update_timer(prev_div) && (ctx.tac & 0b100)){
    ctx.tima++;
    if(ctx.tima == 0)
      tima_overflow = true, ticks_after_overflow = 0;
  }
}

void timer_tick(){
  WORD prev_div = ctx.div;
  ctx.div++;
  tima_tick(prev_div);
}

BYTE timer_read(WORD address){
  switch(address){
    case 0xFF04: return ctx.div >> 8;
    case 0xFF05: return ctx.tima;
    case 0xFF06: return ctx.tma;
    case 0xFF07: return ctx.tac;
  }
  NO_IMPL
  return 0;
}

void tima_glitch(BYTE old_tac){
  bool old_bit = (old_tac & 0b100) && BIT(ctx.div, tac_bit[old_tac & 0b11]);
  bool new_bit = (ctx.tac & 0b100) && BIT(ctx.div, tac_bit[ctx.tac & 0b11]);
  if(old_bit && !new_bit){
    ctx.tima++;
    if(ctx.tima == 0x00)
      tima_overflow = true, ticks_after_overflow = 0;
  }
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
      if(ticks_after_overflow == 4) break;
      ctx.tima = value, ticks_after_overflow = 0, tima_overflow = false;
      break;

    case 0xFF06:
      ctx.tma = value;
      if(ticks_after_overflow == 5) ctx.tima = ctx.tma;
      break;

    case 0xFF07:{
      BYTE old_tac = ctx.tac;
      ctx.tac = value;
      tima_glitch(old_tac);
      break;
    }
  }
}

