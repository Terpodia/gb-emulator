#include <ppu_sm.h>
#include <ppu.h>
#include <ui.h>
#include <lcd.h>
#include <interrupts.h>

uint32_t prev_frame_time;
uint32_t target_frame_time = 1000/60;
uint32_t frame_count;
uint32_t start_timer;

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
    SET_LCD_MODE(MODE_PIXEL_TRANSFER);
    ppu_get_context()->ppu_ticks = 0;
  }
}
void ppu_mode_pixel_transfer(){
  if(ppu_get_context()->ppu_ticks >= PIXEL_TRANSFER_MODE_TICKS){
    SET_LCD_MODE(MODE_HBLANK);
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

      WORD current_frame_time = get_ticks();
      if(current_frame_time - prev_frame_time < target_frame_time){
        delay(target_frame_time - (current_frame_time - prev_frame_time));
      }
      prev_frame_time = get_ticks();
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
