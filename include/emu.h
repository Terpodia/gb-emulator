#pragma once

#include <common.h>

typedef struct {
  bool paused;
  bool running;
  unsigned long long ticks;
} emu_context;

int emu_run(int argc, char **argv);

void emu_cycles(int cpu_cycles);
