#include <ppu/pixel_fifo.h>
#include <ppu/ppu.h>
#include <ppu/lcd.h>
#include <bus.h>

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
  if(pfc->pixel_fifo_size >= 8){
    uint32_t color = pfc->pixel_fifo[pfc->pixel_fifo_head];
    pfc->pixel_fifo_head = (pfc->pixel_fifo_head + 1) % 16;
    pfc->pixel_fifo_size--;

    BYTE y = lcd_get_context()->ly;
    BYTE x = pfc->pushed_x;

    ppu_get_context()->video_buffer[y][x] = color;
    pfc->pushed_x++;
  }
}

uint32_t get_bgw_palette(BYTE color){
  if(!ppu_get_context()->cgb_mode){ 
    if(!BGW_ENABLE) color = 0;
    return lcd_get_context()->dmg_bg_colors[color];
  }
  
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  int palette_index = pfc->bgw_attribute & 0x7;
  int color_index = palette_index * 4 * 2 + color * 2;

  WORD color_rgb5 = 0;
  color_rgb5 += (WORD)lcd_get_context()->cgb_bg_palette[color_index];
  color_rgb5 += (WORD)lcd_get_context()->cgb_bg_palette[color_index + 1] << 8;

  uint32_t r = color_rgb5 & 0x1F;
  uint32_t g = (color_rgb5 >> 5) & 0x1F;
  uint32_t b = (color_rgb5 >> 10) & 0x1F;

  r = (r << 3) | (r >> 2);
  g = (g << 3) | (g >> 2);
  b = (b << 3) | (b >> 2);

  return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

uint32_t get_obj_palette(oam_entry *sprite_entry, BYTE sprite_color){
  if(!ppu_get_context()->cgb_mode){
    if(!sprite_entry->dmg_palette) 
      return lcd_get_context()->dmg_sp1_colors[sprite_color];

    return lcd_get_context()->dmg_sp2_colors[sprite_color];
  }

  int palette_index = sprite_entry->cgb_palette & 0x7;
  int color_index = palette_index * 4 * 2 + sprite_color * 2;
  WORD color_rgb5 = 0;
  color_rgb5 += (WORD)lcd_get_context()->cgb_obj_palette[color_index];
  color_rgb5 += (WORD)lcd_get_context()->cgb_obj_palette[color_index + 1] << 8;

  uint32_t r = color_rgb5 & 0x1F;
  uint32_t g = (color_rgb5 >> 5) & 0x1F;
  uint32_t b = (color_rgb5 >> 10) & 0x1F;

  r = (r << 3) | (r >> 2);
  g = (g << 3) | (g >> 2);
  b = (b << 3) | (b >> 2);

  return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

void pixel_sprite_color(BYTE color, uint32_t &palette){
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

    bool flag = entry.priority || BIT(pfc->bgw_attribute, 7);
    
    if(color != 0 && flag && BGW_ENABLE) continue;

    palette = get_obj_palette(&entry, sprite_color);
    return;
  }
}

bool pixel_fifo_add(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  if(pfc->pixel_fifo_size >= 8) return false;

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

    uint32_t palette = get_bgw_palette(color);

    if(OBJ_ENABLE) pixel_sprite_color(color, palette);

    pfc->pixel_fifo[pfc->pixel_fifo_tail] = palette;
    pfc->pixel_fifo_tail = (pfc->pixel_fifo_tail + 1) % 16;
    pfc->pixel_fifo_size++;

    pfc->fetched_x++;
    pfc->lx++;
  }
  return true;
}

void fetch_sprites_entry(){
  ppu_get_context()->fetched_objects = 0;
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;

  for(int i = 0; i < ppu_get_context()->line_sprites_number; i++){
    auto &entry = ppu_get_context()->line_sprites[i];
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

    if(!ppu_get_context()->cgb_mode) entry.vram_bank = 0;

    pfc->obj_fetched_data[i * 2 + offset] = ppu_get_context()->vram[entry.vram_bank][address - 0x8000];
  }
}

void fetch_bgw_tile(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  
  WORD address = 0;

  if(window_in_range()){
    address = WIN_TILE_MAP_AREA;
    address += (pfc->fetched_x + 7 - lcd_get_context()->wx) / 8;
    address += (ppu_get_context()->window_line / 8) * 32;
    pfc->bgw_fetched_is_window = true;
  }
  else{
    address = BG_TILE_MAP_AREA;
    address += pfc->map_x / 8;
    address += ((WORD)pfc->map_y / 8) * 32;
    pfc->bgw_fetched_is_window = false;
  }

  pfc->bgw_fetched_data[0] = ppu_get_context()->vram[0][address - 0x8000];
  pfc->bgw_attribute = ppu_get_context()->vram[1][address - 0x8000];

  if(BGW_TILE_DATA_AREA == 0x8800) pfc->bgw_fetched_data[0] += 128;
}

void fetch_tile_data(BYTE idx){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;

  int bank = BIT(pfc->bgw_attribute, 3);
  int x_flip = BIT(pfc->bgw_attribute, 5);
  int y_flip = BIT(pfc->bgw_attribute, 6);

  WORD address = BGW_TILE_DATA_AREA;

  if(!y_flip) address += (WORD)pfc->bgw_fetched_data[0] * 16 + pfc->tile_y + idx - 1;
  else address += (WORD)(pfc->bgw_fetched_data[0] + 1) * 16 - pfc->tile_y + idx - 3;

  pfc->bgw_fetched_data[idx] = ppu_get_context()->vram[bank][address - 0x8000];

  if(x_flip){
    BYTE temp = pfc->bgw_fetched_data[idx];
    for(int b = 0; b < 8; b++){
      int val = BIT(temp, 7-b);
      BIT_SET(pfc->bgw_fetched_data[idx], b, val);
    }
  }
}

void pixel_fifo_fetch(){
  pixel_fifo_ctx *pfc = &ppu_get_context()->pfc;
  switch(pfc->pf_state){
    case PFS_GET_TILE:
      fetch_bgw_tile();
      fetch_sprites_entry();
      pfc->pf_state = PFS_GET_TILE_DATA_LOW;
      break;

    case PFS_GET_TILE_DATA_LOW:{
      fetch_tile_data(1);
      fetch_sprites_color(0);

      pfc->pf_state = PFS_GET_TILE_DATA_HIGH;
      break;
    }

    case PFS_GET_TILE_DATA_HIGH:{
      fetch_tile_data(2);
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
