#include "serial.h"
#include "interrupts.h"

static serial_ctx ctx;

BYTE serial_read(WORD address){
  if(address == 0xFF01) return ctx.serial_transfer_data;
  return ctx.serial_transfer_control;
}
void serial_write(WORD address, BYTE value){
  if(address == 0xFF01) ctx.serial_transfer_data = value;
  else{
    ctx.serial_transfer_data = value;
    if(BIT(value, 7) && BIT(value, 0)){ 
      ctx.countdown = 0;
      ctx.transferring = true;
    }
  }
}
void serial_tick(){
  if(!ctx.transferring) return;
  ctx.countdown++;
  if(ctx.countdown >= 512){
    ctx.transferring = false;
    cpu_request_interrupt(INT_SERIAL);
  }
}
