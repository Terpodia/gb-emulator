#pragma once

#include <common.h>
#include <apu/apu_utils.h>

struct square_channel {
  BYTE registers[5];
  BYTE length_counter;
  BYTE wave_step;
  BYTE volume_counter;
  BYTE volume_envelope_counter;
  WORD divider;
  bool sweep_enable;
  WORD shadow_register;

  channel_power_state power;

  square_channel();

  WORD frequency();
  WORD dac();
  BYTE wave_duty();
  BYTE envelope_pace();
  BYTE envelope_dir();
  BYTE sweep_pace();
  BYTE sweep_step();
  BYTE sweep_dir();
  WORD sweep_calculation();
  BYTE initial_volume();
  BYTE initial_length();
  bool length_enable();

  BYTE read_register(square_audio_register reg);
  void write_register(square_audio_register reg, BYTE value);

  void trigger();
  BYTE output();

  void tick();
  void length_tick();
  void envelope_tick();
  void sweep_tick();
};

const BYTE wave_forms[4] = {0b01111111, 0b00111111, 0b00001111, 0b00000011};

