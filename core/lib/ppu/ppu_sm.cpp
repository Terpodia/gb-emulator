#include <ppu/pixel_fifo.h>
#include <ppu/ppu_sm.h>
#include <ppu/ppu.h>
#include <ppu/lcd.h>
#include <ppu/dma.h>
#include <ppu/hdma.h>
#include <interrupts.h>
#include <algorithm>

void compare_lyc(){
  if(lcd_get_context()->ly == lcd_get_context()->lyc){
    if(LCDS_STAT_INT(STAT_INT_MODE_LYC)) cpu_request_interrupt(INT_LCD_STAT);
    LCDS_LYC_SET(1);
  }
  else{
    LCDS_LYC_SET(0);
  }
}

void increment_ly(){
  if(window_is_visible() && lcd_get_context()->ly >= lcd_get_context()->wy
     && ppu_get_context()->window_rendered_this_frame){
    ppu_get_context()->window_line++;
  }
  lcd_get_context()->ly++;
  compare_lyc();
}

void load_line_sprites(){
  oam_entry *line_sprites = ppu_get_context()->line_sprites;
  int &line_sprites_number = ppu_get_context()->line_sprites_number;

  line_sprites_number = 0;
  for(int i = 0; i < 40; i++){
    // at most 10 sprites per line
    if(line_sprites_number >= 10) break;

    BYTE height = LCDS_OBJ_SIZE;
    BYTE ypos = ppu_get_context()->oam[i].y;
    if(ypos > lcd_get_context()->ly + 16) continue;
    if(ypos + height <= lcd_get_context()->ly + 16) continue;

    line_sprites[line_sprites_number++] = ppu_get_context()->oam[i];
  }

  if(lcd_get_context()->object_priority_mode){
    std::stable_sort(line_sprites, line_sprites + line_sprites_number, [](const oam_entry &a, const oam_entry &b){
      return a.x < b.x;
    });
  }
}

void ppu_mode_oam(){
  if(ppu_get_context()->ppu_ticks >= OAM_MODE_TICKS){
    ppu_get_context()->pfc.pushed_x = 0;
    ppu_get_context()->pfc.fetched_x = 0;
    ppu_get_context()->pfc.lx = 0;
    ppu_get_context()->pfc.pf_state = PFS_GET_TILE;

    ppu_get_context()->pfc.pixel_fifo_head = 0;
    ppu_get_context()->pfc.pixel_fifo_tail = 0;
    ppu_get_context()->pfc.pixel_fifo_size = 0;

    SET_LCD_MODE(MODE_PIXEL_TRANSFER);
  }
  if(ppu_get_context()->ppu_ticks == 1){
    if(LCDS_STAT_INT(STAT_INT_MODE_OAM)) cpu_request_interrupt(INT_LCD_STAT);
    load_line_sprites();
  }
}
void ppu_mode_pixel_transfer(){
  pixel_fifo_process();

  if(ppu_get_context()->pfc.pushed_x >= XRES){
    SET_LCD_MODE(MODE_HBLANK);

    if(LCDS_STAT_INT(STAT_INT_MODE_HBLANK))
      cpu_request_interrupt(INT_LCD_STAT);
  }
}
void ppu_mode_hblank(){
  if(ppu_get_context()->ppu_ticks >= LINE_TICKS){
    increment_ly();
    if(lcd_get_context()->ly >= YRES){
      SET_LCD_MODE(MODE_VBLANK);
      cpu_request_interrupt(INT_VBLANK);

      if(LCDS_STAT_INT(STAT_INT_MODE_VBLANK))
        cpu_request_interrupt(INT_LCD_STAT);
    }
    else{
      SET_LCD_MODE(MODE_OAM);
    }
    ppu_get_context()->ppu_ticks = 0;
  }
}
void ppu_mode_vblank(){
  if(ppu_get_context()->ppu_ticks >= LINE_TICKS){
    increment_ly();
    if(lcd_get_context()->ly >= SCANLINES){
      SET_LCD_MODE(MODE_OAM);

      lcd_get_context()->ly = 0;
      ppu_get_context()->window_line = 0;
      ppu_get_context()->window_rendered_this_frame = false;
      ppu_get_context()->current_dot = 0;

      compare_lyc();
      frame_rate_update();
    }
    ppu_get_context()->ppu_ticks = 0;
  }
}
