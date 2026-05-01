#pragma once

#include <common.h>
#include <queue>
#include <vector>

#define OAM_MODE_TICKS 80
#define LINE_TICKS 456
#define XRES 160
#define YRES 144
#define SCANLINES 154
#define MAX_CHECKING_SPRITES_ON_PIXEL 5

struct oam_entry {
  BYTE y;
  BYTE x;
  BYTE tile_index;
  BYTE cgb_palette : 3;
  BYTE vram_bank : 1;
  BYTE dmg_palette : 1;
  BYTE x_flip : 1;
  BYTE y_flip : 1;
  BYTE priority : 1;
};

enum pixel_fifo_state {
  PFS_GET_TILE,
  PFS_GET_TILE_DATA_LOW,
  PFS_GET_TILE_DATA_HIGH,
  PFS_SLEEP,
  PFS_PUSH
};

struct pixel_fifo_ctx {
  uint32_t pixel_fifo[16];
  int pixel_fifo_head;
  int pixel_fifo_tail;
  int pixel_fifo_size;

  pixel_fifo_state pf_state;
  BYTE lx;
  BYTE fetched_x;
  BYTE pushed_x;
  BYTE map_x;
  BYTE map_y;
  BYTE tile_y;

  BYTE bgw_fetched_data[3];
  BYTE bgw_attribute;
  BYTE obj_fetched_data[MAX_CHECKING_SPRITES_ON_PIXEL * 2];

  bool bgw_fetched_is_window;
};

struct ppu_context {
  bool cgb_mode;

  oam_entry oam[40];
  BYTE vram[2][0x2000];

  BYTE vram_current_bank;

  uint32_t video_buffer[YRES][XRES];
  pixel_fifo_ctx pfc;

  oam_entry line_sprites[10];
  int line_sprites_number;

  BYTE fetched_objects;
  oam_entry obj_fetched_entry[MAX_CHECKING_SPRITES_ON_PIXEL];

  WORD ppu_ticks;
  uint32_t current_dot;
  uint64_t current_frame;

  BYTE window_line;
  bool window_rendered_this_frame;

  bool was_on;
};

ppu_context *ppu_get_context();

void ppu_init(bool cgb_mode);

void frame_rate_update();

void ppu_tick();

BYTE ppu_oam_read(WORD address);
void ppu_oam_write(WORD address, BYTE value);

BYTE ppu_vram_read(WORD address);
void ppu_vram_write(WORD address, BYTE value);

BYTE ppu_get_vram_bank();
void ppu_set_vram_bank(BYTE bank_number);
