#pragma once

#include <common.h>
#include <queue>

#define OAM_MODE_TICKS 80
#define PIXEL_TRANSFER_MODE_TICKS 172
#define LINE_TICKS 456
#define XRES 160
#define YRES 144
#define SCANLINES 154

struct oam_entry {
  BYTE y;
  BYTE x;
  BYTE tile_index;
  BYTE cgb_palette : 3;
  BYTE vram_back : 1;
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
  std::queue<uint32_t> pixel_fifo;
  pixel_fifo_state pf_state;
  WORD lx;
  WORD fetched_x;
  WORD pushed_x;
  WORD map_x;
  WORD map_y;
  WORD tile_y;
  BYTE bgw_fetched_data[3];
};

struct ppu_context {
  oam_entry oam[40];
  BYTE vram[0x2000];

  uint32_t video_buffer[YRES][XRES];
  pixel_fifo_ctx pfc;

  WORD ppu_ticks;
  uint64_t current_frame;
};

ppu_context *ppu_get_context();

void ppu_init();

void ppu_tick();

BYTE ppu_oam_read(WORD address);
void ppu_oam_write(WORD address, BYTE value);

BYTE ppu_vram_read(WORD address);
void ppu_vram_write(WORD address, BYTE value);
