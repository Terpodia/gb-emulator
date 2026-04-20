#pragma once

#include <common.h>

#define SAMPLE_SIZE 1024

struct apu_context {
  BYTE audio_master_control;
  BYTE sound_panning;
  BYTE master_volume_and_vin_panning;

  BYTE channel1_registers[5];
  WORD channel1_divider;
  BYTE channel1_wave_step;
  BYTE channel1_volume;
  BYTE channel1_length;
  BYTE channel1_envelope;
  bool channel1_sweep_enable;
  WORD channel1_shadow_register;

  BYTE channel2_registers[4];
  WORD channel2_divider;
  BYTE channel2_wave_step;
  BYTE channel2_volume;
  BYTE channel2_length;
  BYTE channel2_envelope;

  BYTE channel3_registers[5];
  WORD channel3_divider;
  BYTE channel3_wave_step;
  BYTE channel3_output_level;
  BYTE channel3_length;

  BYTE channel4_registers[4];
  uint32_t channel4_divider;
  BYTE channel4_volume;
  BYTE channel4_length;
  BYTE channel4_envelope;
  WORD channel4_lfsr;

  BYTE wave_pattern_ram[16];

  float audio_buffer[SAMPLE_SIZE];
  WORD audio_buffer_index;

  WORD apu_tick;

  WORD frame_sequencer;
  WORD frame_sequencer_counter;
};

apu_context *apu_get_context();
BYTE apu_read(WORD address);
void apu_init();
void apu_write(WORD address, BYTE value);
void apu_tick();

BYTE channel1_output();
BYTE channel2_output();
BYTE channel3_output();
BYTE channel4_output();

#define LEFT_VOLUME ((apu_get_context()->master_volume_and_vin_panning >> 4) & 0b111)
#define RIGHT_VOLUME (apu_get_context()->master_volume_and_vin_panning & 0b111)

#define CHANNEL1_TURN_ON (BIT_SET(apu_get_context()->audio_master_control, 0, 1))
#define CHANNEL1_TURN_OFF (BIT_SET(apu_get_context()->audio_master_control, 0, 0))
#define CHANNEL1_FREQ ((WORD)apu_get_context()->channel1_registers[3] | (((WORD)apu_get_context()->channel1_registers[4] & 0b111) << 8))
#define CHANNEL1_INITIAL_VOLUME (apu_get_context()->channel1_registers[2] >> 4)
#define CHANNEL1_INITIAL_LENGTH (apu_get_context()->channel1_registers[1] & 0b11111)
#define CHANNEL1_ENV_FREQ (apu_get_context()->channel1_registers[2] & 0b111)
#define CHANNEL1_DAC (apu_get_context()->channel1_registers[2] >> 3)
#define CHANNEL1_WAVE_DUTY (apu_get_context()->channel1_registers[1] >> 6)
#define CHANNEL1_ENV_DIR (BIT(apu_get_context()->channel1_registers[3], 3))
#define CHANNEL1_LENGTH_ENABLE (BIT(apu_get_context()->channel1_registers[4], 6))
#define CHANNEL1_LEFT (BIT(apu_get_context()->sound_panning, 4))
#define CHANNEL1_RIGHT (BIT(apu_get_context()->sound_panning, 0))
#define CHANNEL1_ON (BIT(apu_get_context()->audio_master_control, 0))
#define CHANNEL1_SWEEP_PACE ((apu_get_context()->channel1_registers[0] >> 4) & 0b111)
#define CHANNEL1_SWEEP_STEP (apu_get_context()->channel1_registers[0] & 0b111)
#define CHANNEL1_SWEEP_DIR (BIT(apu_get_context()->channel1_registers[0], 3))

#define CHANNEL2_TURN_ON (BIT_SET(apu_get_context()->audio_master_control, 1, 1))
#define CHANNEL2_TURN_OFF (BIT_SET(apu_get_context()->audio_master_control, 1, 0))
#define CHANNEL2_ON (BIT(apu_get_context()->audio_master_control, 1))
#define CHANNEL2_FREQ ((WORD)apu_get_context()->channel2_registers[2] | (((WORD)apu_get_context()->channel2_registers[3] & 0b111) << 8))
#define CHANNEL2_WAVE_DUTY (apu_get_context()->channel2_registers[0] >> 6)
#define CHANNEL2_INITIAL_VOLUME (apu_get_context()->channel2_registers[1] >> 4)
#define CHANNEL2_INITIAL_LENGTH (apu_get_context()->channel2_registers[0] & 0b11111)
#define CHANNEL2_LENGTH_ENABLE (BIT(apu_get_context()->channel2_registers[3], 6))
#define CHANNEL2_LEFT (BIT(apu_get_context()->sound_panning, 5))
#define CHANNEL2_RIGHT (BIT(apu_get_context()->sound_panning, 1))
#define CHANNEL2_ENV_FREQ (apu_get_context()->channel2_registers[1] & 0b111)
#define CHANNEL2_ENV_DIR (BIT(apu_get_context()->channel2_registers[1], 3))
#define CHANNEL2_DAC (apu_get_context()->channel2_registers[1] >> 3)

#define CHANNEL3_ON (BIT(apu_get_context()->audio_master_control, 2))
#define CHANNEL3_TURN_ON (BIT_SET(apu_get_context()->audio_master_control, 2, 1))
#define CHANNEL3_TURN_OFF (BIT_SET(apu_get_context()->audio_master_control, 2, 0))
#define CHANNEL3_FREQ ((WORD)apu_get_context()->channel3_registers[3] | (((WORD)apu_get_context()->channel3_registers[4] & 0b111) << 8))
#define CHANNEL3_OUTPUT_LEVEL ((apu_get_context()->channel3_registers[2] >> 5) & 0b11)
#define CHANNEL3_INITIAL_LENGTH (apu_get_context()->channel3_registers[1])
#define CHANNEL3_LENGTH_ENABLE (BIT(apu_get_context()->channel3_registers[4], 6))
#define CHANNEL3_DAC (BIT(apu_get_context()->channel3_registers[0], 7))
#define CHANNEL3_LEFT (BIT(apu_get_context()->sound_panning, 6))
#define CHANNEL3_RIGHT (BIT(apu_get_context()->sound_panning, 2))

#define CHANNEL4_ON (BIT(apu_get_context()->audio_master_control, 3))
#define CHANNEL4_TURN_ON (BIT_SET(apu_get_context()->audio_master_control, 3, 1))
#define CHANNEL4_TURN_OFF (BIT_SET(apu_get_context()->audio_master_control, 3, 0))
#define CHANNEL4_CLOCK_DIV (apu_get_context()->channel4_registers[2] & 0b111)
#define CHANNEL4_CLOCK_SHIFT (apu_get_context()->channel4_registers[2] >> 4)
#define CHANNEL4_LFSR_WIDTH (BIT(apu_get_context()->channel4_registers[2], 3))
#define CHANNEL4_INITIAL_VOLUME (apu_get_context()->channel4_registers[1] >> 4)
#define CHANNEL4_INITIAL_LENGTH (apu_get_context()->channel4_registers[0] & 0b11111)
#define CHANNEL4_ENV_FREQ (apu_get_context()->channel4_registers[1] & 0b111)
#define CHANNEL4_ENV_DIR (BIT(apu_get_context()->channel4_registers[1], 3))
#define CHANNEL4_LENGTH_ENABLE (BIT(apu_get_context()->channel4_registers[3], 6))
#define CHANNEL4_DAC (apu_get_context()->channel4_registers[1] >> 3)
#define CHANNEL4_LEFT (BIT(apu_get_context()->sound_panning, 7))
#define CHANNEL4_RIGHT (BIT(apu_get_context()->sound_panning, 3))
