#include "quantize.h"

FIBITMAP* liqToBitmap(BYTE* indices, const liq_palette* palette, unsigned int width, unsigned int height) {
    FIBITMAP* bitmapOut = FreeImage_Allocate(width, height, 8);

    RGBQUAD* pal = FreeImage_GetPalette(bitmapOut);
    for (int i = 0; i < 256; i++) {
        pal[i].rgbRed = palette->entries[i].b;
        pal[i].rgbGreen = palette->entries[i].g;
        pal[i].rgbBlue = palette->entries[i].r;
    }

    BYTE* pixels = FreeImage_GetBits(bitmapOut);
    unsigned int pitch = FreeImage_GetPitch(bitmapOut);
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            int offset = y * pitch + x;
            pixels[offset] = indices[y * width + x];
        }
    }

    return bitmapOut;
}

FIBITMAP* quantizeImage(FIBITMAP* inputImage, size_t amount, bool dither) {
    unsigned int width = FreeImage_GetWidth(inputImage);
    unsigned int height = FreeImage_GetHeight(inputImage);
    BYTE* pixels = FreeImage_GetBits(inputImage);

    liq_attr* handle = liq_attr_create();
    liq_set_max_colors(handle, amount);
    liq_set_quality(handle, 100, 0);

    liq_image* image = liq_image_create_rgba(handle, pixels, width, height, 0);
    liq_result* result;
    if (liq_image_quantize(image, handle, &result) != LIQ_OK) return nullptr;

    size_t pixels_size = width * height;
    void* raw_8bit_pixels = malloc(pixels_size);
    liq_set_dithering_level(result, dither ? 1.0 : 0);

    BYTE* quantizedPixels = new BYTE[pixels_size];
    liq_write_remapped_image(result, image, quantizedPixels, pixels_size);
    const liq_palette* palette = liq_get_palette(result);

    return liqToBitmap(quantizedPixels, palette, width, height);
}

FIBITMAP* quantizeImageWP(FIBITMAP* inputImage, CLUT clut, size_t amount, bool dither) {
    // Setup
    unsigned int width = FreeImage_GetWidth(inputImage);
    unsigned int height = FreeImage_GetHeight(inputImage);
    BYTE* pixels = FreeImage_GetBits(inputImage);

    // Quantize Image
    liq_attr* handle = liq_attr_create();
    liq_set_max_colors(handle, amount);
    liq_set_quality(handle, 100, 0);

    liq_image* image = liq_image_create_rgba(handle, pixels, width, height, 0);
    for (RGBQUAD color : clut.colors) {
        liq_color c;
        c.r = color.rgbBlue;
        c.g = color.rgbGreen;
        c.b = color.rgbRed;
        c.a = 255;
        liq_image_add_fixed_color(image, c);
    }

    liq_result* result;
    if (liq_image_quantize(image, handle, &result) != LIQ_OK) return nullptr;

    // Make new pixels
    size_t pixels_size = width * height;
    void* raw_8bit_pixels = malloc(pixels_size);
    liq_set_dithering_level(result, dither ? 1.0 : 0);

    BYTE* quantizedPixels = new BYTE[pixels_size];
    liq_write_remapped_image(result, image, quantizedPixels, pixels_size);
    const liq_palette* palette = liq_get_palette(result);

    // Export result
    return liqToBitmap(quantizedPixels, palette, width, height);
}