#include "color.h"

RGB5 color16to32(uint16_t u16) {
	const unsigned int r = u16 & 0x1f;
	const unsigned int g = (u16 & 0x3e0) >> 5;
	const unsigned int b = (u16 & 0x7c00) >> 10;
	return { r << 3, g << 3, b << 3 };
}

uint16_t color32to16(RGB5 color) {
	const unsigned int r5 = (color.r >> 3) & 0x1f;
	const unsigned int g5 = (color.g >> 3) & 0x1f;
	const unsigned int b5 = (color.b >> 3) & 0x1f;

	const uint16_t result = (0x1 << 15) | (b5 << 10) | (g5 << 5) | r5;
	return result;
}