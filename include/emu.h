#pragma once

#include <common.h>

typedef struct {
  bool paused;
  bool running;
  unsigned long long ticks;
} amu_context;

