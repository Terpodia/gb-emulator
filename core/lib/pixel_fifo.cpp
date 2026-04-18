#include "pixel_fifo.h"
#include "ppu.h"
#include "lcd.h"
#include "bus.h"

bool window_is_visible(){
  return lcd_get_context()->wx < XRES + 7 && lcd_get_context()->wy < YRES && WIN_ENABLE;
}

bool window_in_range(){
  if(!window_is_visible()) return false;

  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;

  if(lcd_get_context()->ly < lcd_get_context()->wy) return false;

  if(pfc->fetched_x + 7 < lcd_get_context()->wx) return false;

  return true;
}

void transfer_pixels(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  if(pfc->pixel_fifo.size() >= 8){
    uint32_t color = pfc->pixel_fifo.front();
    pfc->pixel_fifo.pop();

    BYTE y = lcd_get_context()->ly;
    BYTE x = pfc->pushed_x;

    ppu_get_context()->video_buffer[y][x] = color;
    pfc->pushed_x++;
  }
}

bool pixel_sprite_color(BYTE color, uint32_t &palette){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  for(int i = 0; i < ppu_get_context()->fetched_objects; i++){
    oam_entry entry = ppu_get_context()->obj_fetched_entry[i];
    BYTE xpos = entry.x;

    if(pfc->fetched_x + 8 < xpos) continue;

    if(pfc->fetched_x + 8 >= xpos + 8) continue;

    BYTE bit = 7 - (pfc->fetched_x + 8 - xpos);

    if(entry.x_flip) bit = 7 - bit;

    BYTE lo = BIT(pfc->obj_fetched_data[i * 2], bit);
    BYTE hi = BIT(pfc->obj_fetched_data[i * 2 + 1], bit);

    BYTE sprite_color = (hi << 1) | lo;
    if(sprite_color == 0) continue; // transparent
    
    if(entry.priority && color != 0) continue;

    if(!entry.dmg_palette) palette = lcd_get_context()->sp1_colors[sprite_color];
    else palette = lcd_get_context()->sp2_colors[sprite_color];

    return true;
  }
  return false;
}

bool pixel_fifo_add(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  if(pfc->pixel_fifo.size() >= 8) return false;

  int first_bit = 7;
  if(!pfc->bgw_fetched_is_window && pfc->lx < lcd_get_context()->scx % 8){
    first_bit -= (lcd_get_context()->scx % 8) - pfc->lx;
    pfc->lx = lcd_get_context()->scx % 8;
  }

  for(int x = first_bit; x >= 0; x--){
    if(window_in_range() && !pfc->bgw_fetched_is_window) 
      return true;

    BYTE lo = BIT(pfc->bgw_fetched_data[1], x);
    BYTE hi = BIT(pfc->bgw_fetched_data[2], x);
    BYTE color = (hi << 1) | lo;

    if(!BGW_ENABLE) color = 0;

    uint32_t palette = lcd_get_context()->bg_colors[color];

    if(OBJ_ENABLE) pixel_sprite_color(color, palette);

    pfc->pixel_fifo.push(palette); 
    pfc->fetched_x++;
    pfc->lx++;
  }
  return true;
}

void fetch_sprites_entry(){
  ppu_get_context()->fetched_objects = 0;
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;

  for(auto &entry : ppu_get_context()->line_sprites){
    if(ppu_get_context()->fetched_objects >= MAX_CHECKING_SPRITES_ON_PIXEL) break;

    // out of bonds, not visible
    if(entry.x == 0 || entry.x >= 168) continue;

    BYTE xpos = entry.x;

    if(xpos >= pfc->fetched_x + 8 + 8) continue;
    if(xpos + 8 <= pfc->fetched_x + 8) continue;

    ppu_get_context()->obj_fetched_entry[ppu_get_context()->fetched_objects++] = entry;
  }
}

void fetch_sprites_color(BYTE offset){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  for(int i = 0; i < ppu_get_context()->fetched_objects; i++){
    oam_entry entry = ppu_get_context()->obj_fetched_entry[i];
    BYTE height = LCDS_OBJ_SIZE;

    WORD tile_index = entry.tile_index;
    if(height == 16) tile_index &= ~1;

    WORD sprite_y = lcd_get_context()->ly + 16 - entry.y;
    if(entry.y_flip) sprite_y = height - sprite_y - 1;
    
    sprite_y *= 2;

    WORD address = 0x8000;
    address += tile_index * 16 + sprite_y + offset;

    pfc->obj_fetched_data[i * 2 + offset] = bus_read(address);
  }
}

void fetch_window_tile(){
  if(!window_in_range()) return;

  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;

  WORD address = WIN_TILE_MAP_AREA;
  address += (pfc->fetched_x + 7 - lcd_get_context()->wx) / 8;
  address += (ppu_get_context()->window_line / 8) * 32;

  pfc->bgw_fetched_data[0] = bus_read(address);
  if(BGW_TILE_DATA_AREA == 0x8800) pfc->bgw_fetched_data[0] += 128;

  pfc->bgw_fetched_is_window = true;
}

void pixel_fifo_fetch(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  switch(pfc->pf_state){
    case PFS_GET_TILE:{
      pfc->bgw_fetched_is_window = false;

      WORD address = BG_TILE_MAP_AREA;
      address += pfc->map_x / 8;
      address += ((WORD)pfc->map_y / 8) * 32;
      pfc->bgw_fetched_data[0] = bus_read(address);
      if(BGW_TILE_DATA_AREA == 0x8800) pfc->bgw_fetched_data[0] += 128;

      fetch_window_tile();
      fetch_sprites_entry();

      pfc->pf_state = PFS_GET_TILE_DATA_LOW;

      break;
    }

    case PFS_GET_TILE_DATA_LOW:{
      WORD address = BGW_TILE_DATA_AREA;
      address += (WORD)pfc->bgw_fetched_data[0] * 16 + pfc->tile_y;
      pfc->bgw_fetched_data[1] = bus_read(address);

      fetch_sprites_color(0);

      pfc->pf_state = PFS_GET_TILE_DATA_HIGH;
      break;
    }

    case PFS_GET_TILE_DATA_HIGH:{
      WORD address = BGW_TILE_DATA_AREA;
      address += (WORD)pfc->bgw_fetched_data[0] * 16 + pfc->tile_y + 1;
      pfc->bgw_fetched_data[2] = bus_read(address);

      fetch_sprites_color(1);

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
  pfc->map_x = pfc->lx + lcd_get_context()->scx;

  pfc->tile_y = ((lcd_get_context()->ly + lcd_get_context()->scy) % 8) * 2;

  if(pfc->bgw_fetched_is_window)
    pfc->tile_y = (ppu_get_context()->window_line % 8) * 2;

  if(!(ppu_get_context()->ppu_ticks & 1)){
    pixel_fifo_fetch();
  }

  transfer_pixels();
}
