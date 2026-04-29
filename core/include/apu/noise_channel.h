#include <common.h>
#include <apu/apu_utils.h>

struct noise_channel {
  BYTE registers[4];
  BYTE length_counter;
  BYTE volume_counter;
  BYTE volume_envelope_counter;
  WORD divider;
  WORD lfsr;

  channel_power_state power;

  noise_channel();

  BYTE read_register(noise_audio_register reg);
  void write_register(noise_audio_register reg, BYTE value);

  WORD dac();
  BYTE envelope_pace();
  BYTE envelope_dir();
  BYTE clock_shift();
  BYTE clock_divider();
  BYTE lfsr_width();
  BYTE initial_volume();
  BYTE initial_length();
  bool length_enable();

  void tick();
  void trigger();
  void length_tick();
  void envelope_tick();
  BYTE output();
};
