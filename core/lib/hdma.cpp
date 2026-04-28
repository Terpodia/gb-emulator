#include "hdma.h"
#include "lcd.h"
#include "bus.h"

hdma_context hdma_ctx;

hdma_context* hdma_get_context(){
  return &hdma_ctx;
}

BYTE hdma_remaining_length(){
  if(!hdma_ctx.length) return 0xFF;

  BYTE rem_length = (hdma_ctx.length >> 4) - 1;
  if(!hdma_ctx.transferring) rem_length |= 1<<7;

  return rem_length;
}
void hdma_load_source(WORD address, BYTE value){
  if(address == 0xFF51)
    hdma_ctx.source_address = (hdma_ctx.source_address & 0xFF) | ((WORD)value << 8);

  else
    hdma_ctx.source_address = (hdma_ctx.source_address & 0xFF00) | (WORD)value;
}
void hdma_load_destination(WORD address, BYTE value){
  if(address == 0xFF53)
    hdma_ctx.destination_address = (hdma_ctx.destination_address & 0xFF) | ((WORD)value << 8);

  else
    hdma_ctx.destination_address = (hdma_ctx.destination_address & 0xFF00) | (WORD)value;
}

void hdma_trigger_transfer(BYTE value){
  if(hdma_ctx.transferring && !BIT(value, 7)){
    hdma_ctx.transferring = false;
    return;
  }

  if(BIT(value, 7)) hdma_ctx.mode = HBLANK_DMA;
  else hdma_ctx.mode = GENERAL_PURPOSE_DMA;

  hdma_ctx.source_address &= 0xFFF0;

  hdma_ctx.destination_address &= 0xFFF0;
  hdma_ctx.destination_address &= 0x1FFF;
  hdma_ctx.destination_address += 0x8000;

  hdma_ctx.source_offset = hdma_ctx.source_address;
  hdma_ctx.destination_offset = hdma_ctx.destination_address;

  hdma_ctx.length = value & 0x7F;
  hdma_ctx.length = (hdma_ctx.length + 1) * 0x10;

  hdma_ctx.transferring = true;
  hdma_ctx.already_transferred_in_hblank = false;
  hdma_ctx.transferred_bytes = 0;
}

bool hdma_is_active(){
  if(!hdma_ctx.transferring) return false;
  if(hdma_ctx.mode == GENERAL_PURPOSE_DMA) return true;

  return LCD_MODE == MODE_HBLANK && !hdma_ctx.already_transferred_in_hblank;
}

void hdma_tick(){
  if(LCD_MODE != MODE_HBLANK) 
    hdma_ctx.already_transferred_in_hblank = false;

  if(!hdma_is_active()) return;

  for(int i = 0; i < 2; i++){
    BYTE value = bus_read(hdma_ctx.source_offset);
    bus_write(hdma_ctx.destination_offset, value);

    hdma_ctx.transferred_bytes++;
    hdma_ctx.length--;

    hdma_ctx.source_offset++;
    hdma_ctx.destination_offset++;

    if(hdma_ctx.destination_offset > 0x9FFF)
      hdma_ctx.destination_offset = 0x8000;
  }
  if(!hdma_ctx.length) hdma_ctx.transferring = false;

  if(hdma_ctx.mode == HBLANK_DMA && hdma_ctx.transferred_bytes >= 0x10){
    hdma_ctx.already_transferred_in_hblank = true;
    hdma_ctx.transferred_bytes = 0;
  }
}

