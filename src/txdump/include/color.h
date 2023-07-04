#pragma once
#include <stdint.h>

struct RGB5 {
	unsigned int r;
	unsigned int g;
	unsigned int b;
	unsigned int i;
};

RGB5 color16to32(uint16_t u16);
uint16_t color32to16(RGB5 color);