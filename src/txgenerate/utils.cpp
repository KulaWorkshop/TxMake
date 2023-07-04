#include "utils.h"

RGBQUAD color_16_to_32(uint16_t u16) {
	const unsigned int r = u16 & 0x1f;
	const unsigned int g = (u16 & 0x3e0) >> 5;
	const unsigned int b = (u16 & 0x7c00) >> 10;

	RGBQUAD color;
	color.rgbRed = r << 3;
	color.rgbGreen = g << 3;
	color.rgbBlue = b << 3;
	return color;
}

uint16_t color_32_to_16(RGBQUAD color) {
	const unsigned int r5 = (color.rgbRed >> 3) & 0x1f;
	const unsigned int g5 = (color.rgbGreen >> 3) & 0x1f;
	const unsigned int b5 = (color.rgbBlue >> 3) & 0x1f;

	const uint16_t result = (0x1 << 15) | (b5 << 10) | (g5 << 5) | r5;
	return result;
}

void saveImage(FIBITMAP* image, ghc::filesystem::path path) {
	std::cout << "Writing " << path.string() << std::endl;
	FreeImage_Save(FIF_PNG, image, path.string().c_str());
}