#pragma once
#include "FreeImage.h"

#include "groups.h"

class Tileset {
private:
    FIBITMAP* bitmap;
    unsigned int size;
public:
    FIBITMAP* getTile(unsigned int x, unsigned int y);
    void setTile(FIBITMAP* bitmap, unsigned int x, unsigned int y);

    FIBITMAP* getGroup(unsigned int index);

    void setGroup(FIBITMAP* bitmap, unsigned int index);

    FIBITMAP* getBitmap();

    Tileset(FIBITMAP* bitmap, unsigned int size);
};