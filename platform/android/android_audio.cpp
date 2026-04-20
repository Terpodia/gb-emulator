#include <SDL3/SDL.h>
#include "platform.h"
#include "apu.h"

const float AUDIO_MASTER = 0.5f;

SDL_AudioStream* stream = NULL;

void fill_audio_buffer(){
  apu_context *ctx = apu_get_context();

  float left_volume = (float)(LEFT_VOLUME+1) / 8.0f;
  float right_volume = (float)(RIGHT_VOLUME+1) / 8.0f;
  float left_acc = 0, right_acc = 0, channel_volume = 0;

  if(CHANNEL1_ON){
    channel_volume = (float)channel1_output() / 15.0f;
    if(CHANNEL1_LEFT) left_acc += channel_volume * left_volume;
    if(CHANNEL1_RIGHT) right_acc += channel_volume * right_volume;
  }
  if(CHANNEL2_ON){
    channel_volume = (float)channel2_output() / 15.0f;
    if(CHANNEL2_LEFT) left_acc += channel_volume * left_volume;
    if(CHANNEL2_RIGHT) right_acc += channel_volume * right_volume;
  }
  if(CHANNEL3_ON){
    channel_volume = (float)channel3_output() / 15.f;
    if(CHANNEL3_LEFT) left_acc += channel_volume * left_volume;
    if(CHANNEL3_RIGHT) right_acc += channel_volume * right_volume;
  }
  if(CHANNEL4_ON){
    channel_volume = (float)channel4_output() / 15.0f;
    if(CHANNEL4_LEFT) left_acc += channel_volume * left_volume;
    if(CHANNEL4_RIGHT) right_acc += channel_volume * right_volume;
  }
  left_acc /= 4.0f, right_acc /= 4.0f;

  ctx->audio_buffer[ctx->audio_buffer_index++] = left_acc * AUDIO_MASTER;
  ctx->audio_buffer[ctx->audio_buffer_index++] = right_acc * AUDIO_MASTER;
}

void platform_audio_init(int samples_size){
  SDL_AudioSpec audio_spec = {};
  audio_spec.freq = 44100;
  audio_spec.format = SDL_AUDIO_F32;
  audio_spec.channels = 2;

  stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, NULL, NULL);
  SDL_ResumeAudioStreamDevice(stream);
}

void platform_audio_queue(float *samples, int samples_size){
  while(SDL_GetAudioStreamQueued(stream) > samples_size * sizeof(float))
    SDL_Delay(1);

  SDL_PutAudioStreamData(stream, samples, samples_size * sizeof(float));
}

