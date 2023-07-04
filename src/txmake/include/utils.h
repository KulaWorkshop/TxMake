#pragma once
#include <stdint.h>
#include <stdio.h>

#include "FreeImage.h"

RGBQUAD color_16_to_32(uint16_t data);
uint16_t color_32_to_16(RGBQUAD color);