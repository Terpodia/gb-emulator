#pragma once

#include <common.h>

struct dma_context {
  WORD start_address;
  WORD address_offset;
  BYTE start_delay;
  bool active;
};

void dma_start(WORD address);

void dma_tick();

bool dma_is_active();
