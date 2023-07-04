#pragma once
#include "FreeImage.h"
#include "clut.h"

#include <cstring>

enum class BPP {
	BPP8 = 8,
	BPP4 = 4
};

class Texture {
private:
	int width;
	int height;
	BPP bpp;
public:
	Texture(int width, int height, BPP bpp);

	FIBITMAP* toBitmap(CLUT& clut, unsigned char* buffer);
};