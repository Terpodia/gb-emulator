#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <iostream>

typedef __int8_t SIGNED_BYTE;
typedef __uint8_t BYTE;
typedef __int16_t SIGNED_WORD;
typedef __uint16_t WORD;

#define BIT(a, n) ((a & (1 << n)) ? 1 : 0)

#define BIT_SET(a, n, on) (on ? (a) |= (1 << n) : (a) &= ~(1 << n))

#define NO_IMPL { std::cout << "NOT YET IMPLEMENTED\n"; exit(-5); }
