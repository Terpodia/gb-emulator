#include <pixel_fifo.h>
#include <ppu.h>
#include <lcd.h>
#include <bus.h>

void transfer_pixels(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  if(pfc->pixel_fifo.size() > 8){
    uint32_t color = pfc->pixel_fifo.front();
    pfc->pixel_fifo.pop();

    if(pfc->lx >= (pfc->map_x) % 8){
      BYTE y = lcd_get_context()->ly;
      BYTE x = pfc->pushed_x;

      ppu_get_context()->video_buffer[y][x] = color;
      pfc->pushed_x++;
    }
    pfc->lx++;
  }
}

bool pixel_fifo_add(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  if(pfc->pixel_fifo.size() > 8) return false;

  int xx = ppu_get_context()->pfc.fetched_x - (8 - (lcd_get_context()->scx % 8));
  for(int x = 7; x >= 0; x--){
    BYTE lo = BIT(pfc->bgw_fetched_data[1], x);
    BYTE hi = BIT(pfc->bgw_fetched_data[2], x);
    BYTE color = (hi << 1) | lo;

    if(BGW_ENABLE)
      pfc->pixel_fifo.push(lcd_get_context()->bg_colors[color]);
    else
      pfc->pixel_fifo.push(0xFFFFFFFF);
  }
  return true;
}

void pixel_fifo_fetch(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  switch(pfc->pf_state){
    case PFS_GET_TILE:{
      WORD address = BG_TILE_MAP_AREA;
      address += pfc->map_x / 8;
      address += (pfc->map_y / 8) * 32;
      pfc->bgw_fetched_data[0] = bus_read(address);
      if(BGW_TILE_DATA_AREA == 0x8800) pfc->bgw_fetched_data[0] += 128;

      pfc->pf_state = PFS_GET_TILE_DATA_LOW;
      pfc->fetched_x += 8;

      break;
    }

    case PFS_GET_TILE_DATA_LOW:{
      WORD address = BGW_TILE_DATA_AREA;
      address += pfc->bgw_fetched_data[0] * 16 + pfc->tile_y;
      pfc->bgw_fetched_data[1] = bus_read(address);
      pfc->pf_state = PFS_GET_TILE_DATA_HIGH;
      break;
    }

    case PFS_GET_TILE_DATA_HIGH:{
      WORD address = BGW_TILE_DATA_AREA;
      address += pfc->bgw_fetched_data[0] * 16 + pfc->tile_y + 1;
      pfc->bgw_fetched_data[2] = bus_read(address);
      pfc->pf_state = PFS_SLEEP;
      break;
    }

    case PFS_SLEEP:
      pfc->pf_state = PFS_PUSH;
      break;

    case PFS_PUSH:
      if(pixel_fifo_add()) pfc->pf_state = PFS_GET_TILE;
      break;
  }
}

void pixel_fifo_process(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  pfc->map_y = lcd_get_context()->ly + lcd_get_context()->scy;
  pfc->map_x = pfc->fetched_x + lcd_get_context()->scx;
  pfc->tile_y = ((lcd_get_context()->ly + lcd_get_context()->scy) % 8) * 2;

  if(!(ppu_get_context()->ppu_ticks & 1)){
    pixel_fifo_fetch();
  }

  transfer_pixels();
}
