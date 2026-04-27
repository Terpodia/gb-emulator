#pragma once

#include <common.h>

typedef struct {
  bool paused;
  bool running;
  bool quit;
} emu_context;

int emu_run(int argc, char **argv);

void emu_cycles(int cpu_cycles);

emu_context *emu_get_context();
