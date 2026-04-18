#include "platform.h"
#include "apu.h"
#include <SDL2/SDL.h>

void fill_audio_buffer(){
  apu_context *ctx = apu_get_context();

  int left_volume = (128 * (LEFT_VOLUME)) / 8;
  int right_volume = (128 * (RIGHT_VOLUME)) / 8;
  float left_acc = 0, right_acc = 0, channel_volume = 0;

  if(CHANNEL1_ON && CHANNEL1_LEFT){
    channel_volume = (float)channel1_output() / 100.0f;
    SDL_MixAudioFormat((BYTE *)&left_acc, (BYTE *)&channel_volume, AUDIO_F32SYS, sizeof(float), left_volume);
  }
  if(CHANNEL2_ON && CHANNEL2_LEFT){
    channel_volume = (float)channel2_output() / 100.0f;
    SDL_MixAudioFormat((BYTE *)&left_acc, (BYTE *)&channel_volume, AUDIO_F32SYS, sizeof(float), left_volume);
  }
  if(CHANNEL3_ON && CHANNEL3_LEFT){
    channel_volume = (float)channel3_output() / 100.0f;
    SDL_MixAudioFormat((BYTE *)&left_acc, (BYTE *)&channel_volume, AUDIO_F32SYS, sizeof(float), left_volume);
  }
  if(CHANNEL4_ON && CHANNEL4_LEFT){
    channel_volume = (float)channel4_output() / 100.0f;
    SDL_MixAudioFormat((BYTE *)&left_acc, (BYTE *)&channel_volume, AUDIO_F32SYS, sizeof(float), left_volume);
  }

  ctx->audio_buffer[ctx->audio_buffer_index++] = left_acc;

  if(CHANNEL1_ON && CHANNEL1_RIGHT){
    channel_volume = (float)channel1_output() / 100.0f;
    SDL_MixAudioFormat((BYTE *)&right_acc, (BYTE *)&channel_volume, AUDIO_F32SYS, sizeof(float), right_volume);
  }
  if(CHANNEL2_ON && CHANNEL2_RIGHT){
    channel_volume = (float)channel2_output() / 100.0f;
    SDL_MixAudioFormat((BYTE *)&right_acc, (BYTE *)&channel_volume, AUDIO_F32SYS, sizeof(float), right_volume);
  }
  if(CHANNEL3_ON && CHANNEL3_RIGHT){
    channel_volume = (float)channel3_output() / 100.0f;
    SDL_MixAudioFormat((BYTE *)&right_acc, (BYTE *)&channel_volume, AUDIO_F32SYS, sizeof(float), right_volume);
  }
  if(CHANNEL4_ON && CHANNEL4_RIGHT){
    channel_volume = (float)channel4_output() / 100.0f;
    SDL_MixAudioFormat((BYTE *)&right_acc, (BYTE *)&channel_volume, AUDIO_F32SYS, sizeof(float), right_volume);
  }

  ctx->audio_buffer[ctx->audio_buffer_index++] = right_acc;
}

void platform_audio_init(int samples_size){
  SDL_AudioSpec audio_spec;
  audio_spec.freq = 44100;
  audio_spec.format = AUDIO_F32SYS;
  audio_spec.channels = 2;
  audio_spec.samples = samples_size;
  audio_spec.callback = NULL;
  SDL_AudioSpec audio_obtained;
  SDL_OpenAudio(&audio_spec, &audio_obtained);
	SDL_PauseAudio(0);
}

void platform_audio_queue(float *samples, int samples_size){
  while(SDL_GetQueuedAudioSize(1) > samples_size * sizeof(float)) SDL_Delay(1);
  SDL_QueueAudio(1, samples, sizeof(float) * samples_size);
}
