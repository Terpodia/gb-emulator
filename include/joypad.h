#include <common.h>

struct joypad_state {
  bool start;
  bool select;
  bool up;
  bool right;
  bool down;
  bool left;
  bool a;
  bool b;
};

struct joypad_ctx {
  bool select_button;
  bool select_direction;
  joypad_state state;
};

void joypad_write(BYTE value);
BYTE joypad_read();
joypad_ctx *joypad_get_context();
