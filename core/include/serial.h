#include <common.h>

struct serial_ctx {
  BYTE serial_transfer_data;
  BYTE serial_transfer_control;
  WORD countdown;
  bool transferring;
};

BYTE serial_read(WORD address);
void serial_write(WORD address, BYTE value);
void serial_tick();
