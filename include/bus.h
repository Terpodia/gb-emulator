#pragma once

#include <common.h>

BYTE bus_read(WORD address);

void bus_write(WORD address, BYTE value);
