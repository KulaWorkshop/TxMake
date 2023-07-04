#pragma once

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <fstream>

#include "common.h"
#include "utils.h"

class CLUT {
private:
    size_t size;
public:
    std::vector<RGBQUAD> colors;
    bool addBitmap(FIBITMAP* bitmap);
    bool fromBitmap(FIBITMAP* bitmap);
    FIBITMAP* toBitmap();

    unsigned char* toBuffer(size_t* sizeOut);
    CLUT();

    void setSize(size_t size);
};