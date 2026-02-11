#include <common.h>

#define VBLANK 1
#define LCD 2
#define TIMER 4
#define SERIAL 8
#define JOYPAD 16

void cpu_handle_interrupts();
void cpu_request_interrupt(BYTE interrupt_type);
