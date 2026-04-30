#pragma once

#include <common.h>
#include <atomic>
#include <vector>

enum square_audio_register {
  SQUARE_SWEEP_REGISTER,
  SQUARE_LENGTH_AND_WAVE_DUTY_REGISTER,
  SQUARE_VOLUME_AND_ENVELOPE_REGISTER,
  SQUARE_PERIOD_LOW_REGISTER,
  SQUARE_PERIOD_HIGH_AND_CONTROL_REGISTER
};

enum wave_audio_register {
  WAVE_DAC_REGISTER,
  WAVE_LENGTH_REGISTER,
  WAVE_OUTPUT_LEVEL_REGISTER,
  WAVE_PERIOD_LOW_REGISTER,
  WAVE_PERIOD_HIGH_AND_CONTROL_REGISTER
};

enum noise_audio_register {
  NOISE_LENGTH_REGISTER,
  NOISE_VOLUME_AND_ENVELOPE_REGISTER,
  NOISE_FREQUENCY_AND_RANDOMNESS_REGISTER,
  NOISE_CONTROL_REGISTER
};

enum channel_power_state {
  POWER_ON, POWER_OFF
};

struct ring_buffer {
  std::vector<float> buff;
  std::atomic<int> write_pos{0}, read_pos{0};
  int capacity;

  ring_buffer(int c = 0) : buff(c), capacity(c) {}

  int write(float value);
  int read(float *data, int sz);
  int available_space();
};

#define WAVE_RAM_START 0xFF30

#define LEFT_VOLUME ((apu_get_context()->master_volume_and_vin_panning >> 4) & 0b111)
#define RIGHT_VOLUME (apu_get_context()->master_volume_and_vin_panning & 0b111)

#define CHANNEL1_LEFT_ON (BIT(apu_get_context()->sound_panning, 4))
#define CHANNEL1_RIGHT_ON (BIT(apu_get_context()->sound_panning, 0))

#define CHANNEL2_LEFT_ON (BIT(apu_get_context()->sound_panning, 5))
#define CHANNEL2_RIGHT_ON (BIT(apu_get_context()->sound_panning, 1))

#define CHANNEL3_LEFT_ON (BIT(apu_get_context()->sound_panning, 6))
#define CHANNEL3_RIGHT_ON (BIT(apu_get_context()->sound_panning, 2))

#define CHANNEL4_LEFT_ON (BIT(apu_get_context()->sound_panning, 7))
#define CHANNEL4_RIGHT_ON (BIT(apu_get_context()->sound_panning, 3))
