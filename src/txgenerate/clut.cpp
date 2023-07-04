#include "clut.h"

bool CLUT::addBitmap(FIBITMAP* bitmap) {
    unsigned int width = FreeImage_GetWidth(bitmap);
    unsigned int height = FreeImage_GetHeight(bitmap);
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            RGBQUAD color;
            FreeImage_GetPixelColor(bitmap, x, y, &color);

            bool color_found = false;
            for (auto& palette_color : this->colors) {
                if (palette_color.rgbRed == color.rgbRed &&
                    palette_color.rgbGreen == color.rgbGreen &&
                    palette_color.rgbBlue == color.rgbBlue) {
                    color_found = true;
                    break;
                }
            }

            if (!color_found) {
                this->colors.push_back(color);
            }
        }
    }

    return true;
}

bool CLUT::fromBitmap(FIBITMAP* bitmap) {
    unsigned int width = FreeImage_GetWidth(bitmap);
    for (unsigned int i = 0; i < width; i++) {
        RGBQUAD color;
        FreeImage_GetPixelColor(bitmap, i, 0, &color);
        this->colors.push_back(color);
    }

    return true;
}

FIBITMAP* CLUT::toBitmap() {
    FIBITMAP* bitmap = FreeImage_Allocate(this->colors.size(), 1, 32);

    int i = 0;
    for (RGBQUAD color : this->colors) {
        FreeImage_SetPixelColor(bitmap, i, 0, &color);
        i++;
    }

    return bitmap;
}

unsigned char* CLUT::toBuffer(size_t* sizeOut) {
    if (this->colors.size() != this->size) {
        printf("Error\n");
    }

    const size_t size = this->size * 2;
    unsigned char* buffer = (unsigned char*)malloc(size);

    unsigned int index = 0;
    for (RGBQUAD color : this->colors) {
        uint16_t u16Color = color_32_to_16(color);
        buffer[index] = (unsigned char)(u16Color & 0xff);
        buffer[index + 1] = (unsigned char)((u16Color >> 8) & 0xff);
        index += 2;
    }

    *sizeOut = size;
    return buffer;
}

CLUT::CLUT() {
    this->size = 256;
}

void CLUT::setSize(size_t size) {
    this->size = size;
}