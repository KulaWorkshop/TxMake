#include "texture.h"

FIBITMAP* Texture::toBitmap(CLUT& clut, unsigned char* buffer) {
	FIBITMAP* image = FreeImage_Allocate(width, height, 8);
	RGBQUAD* palette = FreeImage_GetPalette(image);
	for (unsigned int i = 0; i < clut.size; i++) {
		RGB5 c = clut.colors[i];
		RGBQUAD color = { c.b, c.g, c.r };
		palette[i] = color;
	}

	BYTE* bits = FreeImage_GetBits(image);

	if (bpp == BPP::BPP8)
		std::memcpy(bits, buffer, (size_t)(width * height));
	else {
		unsigned int bitIndex = 0;
		for (int i = 0; i < width * height; i += 2) {
			unsigned char byte = buffer[i / 2];
			bits[i] = byte % 16;
			bits[i + 1] = byte / 16;
		}
	}

	FreeImage_FlipVertical(image);
	return image;
}

Texture::Texture(int width, int height, BPP bpp) : width(width), height(height), bpp(bpp) {}