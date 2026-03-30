#include <ppu.h>
#include <ppu_sm.h>
#include <lcd.h>
#include <ui.h>
#include <cstring>

static ppu_context ctx;

ppu_context *ppu_get_context(){
  return &ctx;
}

void ppu_init(){
  lcd_init();
  SET_LCD_MODE(MODE_OAM);
  ctx.ppu_ticks = 0;
  ctx.current_frame = 0;
  ctx.window_line = 0;

  memset(ctx.video_buffer, 0, sizeof(ctx.video_buffer));
  memset(ctx.oam, 0, sizeof(ctx.oam));
  memset(ctx.vram, 0, sizeof(ctx.vram));
}

static uint64_t prev_frame_time;
static uint64_t target_frame_time = 1000/60;
static uint64_t frame_count;
static uint64_t start_timer;

void frame_rate_update(){
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

void ppu_tick(){
  if(!(lcd_get_context()->lcdc & 0x80)) {
    lcd_get_context()->off_clock++;

    if(lcd_get_context()->off_clock >= 70224){
      lcd_get_context()->off_clock = 0;
      frame_rate_update();
    }

    ctx.ppu_ticks = 0;
    return;
  }

  ctx.ppu_ticks++;

  switch(LCD_MODE){
    case MODE_OAM:
      ppu_mode_oam();
      break;

    case MODE_PIXEL_TRANSFER:
      ppu_mode_pixel_transfer();
      break;

    case MODE_HBLANK:
      ppu_mode_hblank();
      break;

    case MODE_VBLANK:
      ppu_mode_vblank();
      break;
  }
}

BYTE ppu_oam_read(WORD address){
  BYTE *oam = (BYTE *)ctx.oam;
  return oam[address - 0xFE00];
}
void ppu_oam_write(WORD address, BYTE value){
  BYTE *oam = (BYTE *)ctx.oam;
  oam[address - 0xFE00] = value;
}

BYTE ppu_vram_read(WORD address){
  return ctx.vram[address - 0x8000];
}
void ppu_vram_write(WORD address, BYTE value){
  ctx.vram[address - 0x8000] = value;
}
