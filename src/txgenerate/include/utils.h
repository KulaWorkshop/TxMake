#pragma once
#include <iostream>
#include <stdint.h>
#include <stdio.h>

#include "common.h"

RGBQUAD color_16_to_32(uint16_t data);
uint16_t color_32_to_16(RGBQUAD color);
void saveImage(FIBITMAP* image, ghc::filesystem::path path);