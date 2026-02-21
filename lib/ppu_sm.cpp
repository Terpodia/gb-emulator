#include <pixel_fifo.h>
#include <interrupts.h>
#include <ppu_sm.h>
#include <lcd.h>
#include <ppu.h>
#include <ui.h>

uint64_t prev_frame_time;
uint64_t target_frame_time = 1000/60;
uint64_t frame_count;
uint64_t start_timer;

void increment_ly(){
  lcd_get_context()->ly++;
  if(lcd_get_context()->ly == lcd_get_context()->lyc){
    if(LCDS_STAT_INT(STAT_INT_MODE_LYC)) cpu_request_interrupt(INT_LCD_STAT);
    LCDS_LYC_SET(1);
  }
  else{
    LCDS_LYC_SET(0);
  }
}

void ppu_mode_oam(){
  if(ppu_get_context()->ppu_ticks >= OAM_MODE_TICKS){
    ppu_get_context()->pfc.pushed_x = 0;
    ppu_get_context()->pfc.fetched_x = 0;
    ppu_get_context()->pfc.lx = 0;
    ppu_get_context()->pfc.pf_state = PFS_GET_TILE;

    while(!ppu_get_context()->pfc.pixel_fifo.empty())
      ppu_get_context()->pfc.pixel_fifo.pop();

    SET_LCD_MODE(MODE_PIXEL_TRANSFER);
    ppu_get_context()->ppu_ticks = 0;
  }
}
void ppu_mode_pixel_transfer(){
  pixel_fifo_process();

  if(ppu_get_context()->pfc.pushed_x >= XRES){
    SET_LCD_MODE(MODE_HBLANK);

    if(LCDS_STAT_INT(STAT_INT_MODE_HBLANK))
      cpu_request_interrupt(INT_LCD_STAT);
    
    ppu_get_context()->ppu_ticks = 0;
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

      uint64_t current_frame_time = get_ticks();

      if(current_frame_time - prev_frame_time < target_frame_time){
        uint64_t delta = target_frame_time - (current_frame_time - prev_frame_time);
        delay(delta);
        prev_frame_time = current_frame_time + delta;
      }
      else prev_frame_time = current_frame_time;

      frame_count++;
      ppu_get_context()->current_frame++;

      if(prev_frame_time - start_timer >= 1000){
        std::cout << "FPS: " << frame_count << "\n";
        start_timer = prev_frame_time;
        frame_count = 0;
      }
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
    }

    ppu_get_context()->ppu_ticks = 0;
  }
}
