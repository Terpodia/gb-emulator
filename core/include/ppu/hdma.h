#include "common.h"

enum HDMA_MODE {GENERAL_PURPOSE_DMA, HBLANK_DMA};

struct hdma_context {
  WORD source_address;
  WORD source_offset;
  WORD destination_address;
  WORD destination_offset;
  WORD length;

  bool transferring;
  bool already_transferred_in_hblank;

  HDMA_MODE mode;
  BYTE transferred_bytes;
};

hdma_context* hdma_get_context();

BYTE hdma_remaining_length();

void hdma_load_source(WORD address, BYTE value);
void hdma_load_destination(WORD address, BYTE value);
void hdma_trigger_transfer(BYTE value);

bool hdma_is_active();
void hdma_tick();
