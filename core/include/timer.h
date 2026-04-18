#include <common.h>

struct timer_context {
  WORD div;
  BYTE tima;
  BYTE tma;
  BYTE tac;
};

void timer_init();
void timer_tick();

BYTE timer_read(WORD address);
void timer_write(WORD address, BYTE value);
