#pragma once

#include <common.h>

uint32_t get_ticks();
void delay(uint32_t ms);

void ui_init();
void ui_update();
void ui_handle_events();
void ui_quit();
