#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef __int8_t BYTE;
typedef __uint8_t UNSIGNED_BYTE;
typedef __int16_t WORD;
typedef __int16_t UNSIGNED_WORD;

#define BIT(a, n) ((a & (1 << n)) ? 1 : 0)
