#include <common.h>

#define INT_VBLANK 1
#define INT_LCD_STAT 2
#define INT_TIMER 4
#define INT_SERIAL 8
#define INT_JOYPAD 16

void cpu_handle_interrupts();
void cpu_request_interrupt(BYTE interrupt_type);
