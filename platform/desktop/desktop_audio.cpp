#include <SDL3/SDL.h>
#include "platform.h"
#include "apu/apu.h"
#include <unistd.h>

const float AUDIO_MASTER = 0.5f;

SDL_AudioStream* stream = NULL;

float channel_output[4];
BYTE left_on[4], right_on[4];

void fill_audio_buffer(){
  apu_context *ctx = apu_get_context();

  square_channel &channel1 = ctx->channel1;
  square_channel &channel2 = ctx->channel2;
  wave_channel &channel3 = ctx->channel3;
  noise_channel &channel4 = ctx->channel4;

  channel_output[0] = (float)channel1.output() / 15.0f;
  channel_output[1] = (float)channel2.output() / 15.0f;
  channel_output[2] = (float)channel3.output() / 15.0f;
  channel_output[3] = (float)channel4.output() / 15.0f;

  left_on[0] = CHANNEL1_LEFT_ON;
  left_on[1] = CHANNEL2_LEFT_ON;
  left_on[2] = CHANNEL3_LEFT_ON;
  left_on[3] = CHANNEL4_LEFT_ON;

  right_on[0] = CHANNEL1_RIGHT_ON;
  right_on[1] = CHANNEL2_RIGHT_ON;
  right_on[2] = CHANNEL3_RIGHT_ON;
  right_on[3] = CHANNEL4_RIGHT_ON;

  float left_volume = (float)(LEFT_VOLUME + 1) / 8.0f;
  float right_volume = (float)(RIGHT_VOLUME + 1) / 8.0f;
  float total_left_volume = 0, total_right_volume = 0;

  for(int i = 0; i < 4; i++){
    total_left_volume += channel_output[i] * left_volume * left_on[i];
    total_right_volume += channel_output[i] * right_volume * right_on[i];
  }

  total_left_volume /= 4.0f, total_right_volume /= 4.0f;

  while(ctx->audio_buffer.available_space() < 2){
    usleep(1);
  }

  ctx->audio_buffer.write(total_left_volume * AUDIO_MASTER);
  ctx->audio_buffer.write(total_right_volume * AUDIO_MASTER);
}

void SDLCALL audio_callback(void *user_data, SDL_AudioStream* stream, int addition_amount, int sz){
  int floats_needed = addition_amount / sizeof(float);
  float buf[SAMPLE_SIZE];
  int n = std::min(floats_needed, SAMPLE_SIZE);

  int got = apu_get_context()->audio_buffer.read(buf, n);

  SDL_PutAudioStreamData(stream, buf, got * sizeof(float));
  floats_needed -= got;
}

void platform_audio_init(int samples_size){
  SDL_AudioSpec audio_spec = {};
  audio_spec.freq = 48000;
  audio_spec.format = SDL_AUDIO_F32;
  audio_spec.channels = 2;

  stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, audio_callback, NULL);
  SDL_ResumeAudioStreamDevice(stream);
}

