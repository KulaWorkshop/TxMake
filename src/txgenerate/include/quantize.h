#pragma once
#include <cstdlib>

#include "clut.h"
#include "libimagequant.h"

#include "common.h"

FIBITMAP* quantizeImage(FIBITMAP* image, size_t amount, bool dither);
FIBITMAP* quantizeImageWP(FIBITMAP* inputImage, CLUT clut, size_t amount, bool dither);