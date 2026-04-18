#include "joypad.h"

static joypad_ctx ctx;

void joypad_write(BYTE value){
  ctx.select_direction = BIT(value, 4);
  ctx.select_button = BIT(value, 5);
}

BYTE joypad_read(){
  BYTE res = 0xCF;
  if(!ctx.select_button){
    if(ctx.state.start) res &= ~(1<<3);
    if(ctx.state.select) res &= ~(1<<2);
    if(ctx.state.b) res &= ~(1<<1);
    if(ctx.state.a) res &= ~(1<<0);
  }
  if(!ctx.select_direction){
    if(ctx.state.down) res &= ~(1<<3);
    if(ctx.state.up) res &= ~(1<<2);
    if(ctx.state.left) res &= ~(1<<1);
    if(ctx.state.right) res &= ~(1<<0);
  }
  return res;
}

joypad_ctx *joypad_get_context(){
  return &ctx;
}
