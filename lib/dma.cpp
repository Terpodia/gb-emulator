#include <dma.h>
#include <ppu.h>
#include <bus.h>
#include <unistd.h>

const int OAM_START = 0xFE00;

static dma_context ctx;

void dma_start(WORD address){
  ctx.start_address = address << 8;
  ctx.address_offset = 0;
  ctx.start_delay = 0;
  ctx.active = true;
}

void dma_tick(){
  if(!ctx.active) return;
  if(ctx.start_delay){
    ctx.start_delay--;
    return;
  }
  ppu_oam_write(OAM_START + ctx.address_offset, bus_read(ctx.start_address + ctx.address_offset));
  ctx.address_offset++;
  ctx.active = ctx.address_offset < 0xA0;
}

bool dma_is_active(){
  return ctx.active;
}
