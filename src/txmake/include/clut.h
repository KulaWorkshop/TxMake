#pragma once

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <fstream>

#include "FreeImage.h"
#include "utils.h"
#include "segments.h"

struct Indexed {
    std::vector<int> indexed;
    Segment segment;
};

class CLUT {
private:
    size_t size;
public:
    std::vector<Indexed> indexed;

    std::vector<RGBQUAD> colors;
    bool addBitmap(FIBITMAP* bitmap);
    bool addBitmap2(FIBITMAP* bitmap, Segment& segment);
    bool fromBitmap(FIBITMAP* bitmap);
    FIBITMAP* toBitmap();

    unsigned char* toBuffer(size_t* sizeOut);
    CLUT();

    void setSize(size_t size);

    void indexImage(FIBITMAP* image, Segment& segment);
};