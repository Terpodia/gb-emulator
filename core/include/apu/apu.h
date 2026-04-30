#pragma once

#include <common.h>
#include <apu/apu_utils.h>
#include <apu/square_channel.h>
#include <apu/wave_channel.h>
#include <apu/noise_channel.h>

#define SAMPLE_SIZE 4096

struct apu_context {
  BYTE audio_master_control;
  BYTE sound_panning;
  BYTE master_volume_and_vin_panning;

  square_channel channel1;
  square_channel channel2;
  wave_channel channel3;
  noise_channel channel4;

  BYTE wave_pattern_ram[16];

  ring_buffer audio_buffer{SAMPLE_SIZE * 4};

  WORD apu_tick;

  WORD frame_sequencer;
  WORD frame_sequencer_counter;
};

apu_context *apu_get_context();
BYTE apu_read(WORD address);
void apu_init();
void apu_write(WORD address, BYTE value);
void apu_tick();

