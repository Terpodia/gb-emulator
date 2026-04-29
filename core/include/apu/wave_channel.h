#include <common.h>
#include <apu/apu_utils.h>

struct wave_channel {
  BYTE registers[5];

  WORD length_counter;
  WORD divider;
  BYTE wave_step;

  channel_power_state power;

  wave_channel();
  BYTE read_register(wave_audio_register reg);
  void write_register(wave_audio_register reg, BYTE value);

  BYTE output_level();
  WORD frequency();
  bool length_enable();

  void trigger();
  void tick();
  void length_tick();
  BYTE output();
};

