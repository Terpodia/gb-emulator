#pragma once

#include <common.h>

uint32_t get_ticks();
void delay(uint32_t ms);

void platform_init();
void platform_render_update();
void platform_poll_events();
void platform_quit();

void fill_audio_buffer();
void platform_audio_init(int samples_size);
void platform_audio_queue(float *samples, int samples_size);

