#include "tileset.h"

FIBITMAP* Tileset::getTile(unsigned int x, unsigned int y) {
    return FreeImage_Copy(bitmap, x * size, y * size, (x * size) + size, (y * size) + size);
}

void Tileset::setTile(FIBITMAP* bitmap, unsigned int x, unsigned int y) {
    FreeImage_Paste(this->bitmap, bitmap, x * size, y * size, 256);
}

FIBITMAP* Tileset::getGroup(unsigned int index) {
    unsigned int width = 0;

    for (int i = 0; i < 56; i++) {
        const Group group = groups[i];
        if (index != group.index) continue;
        width += size;
    }

    FIBITMAP* groupBitmap = FreeImage_Allocate(width, size, 32);

    unsigned int left = 0;
    for (int i = 0; i < 56; i++) {
        const Group group = groups[i];
        if (index != group.index) continue;

        FIBITMAP* b = getTile(group.x, group.y);
        FreeImage_Paste(groupBitmap, b, left, 0, 256);
        left += size;
    }

    return groupBitmap;
}

void Tileset::setGroup(FIBITMAP* bitmap, unsigned int index) {

    unsigned int left = 0;
    for (int i = 0; i < 56; i++) {
        const Group group = groups[i];
        if (index != group.index) continue;

        FIBITMAP* tile = FreeImage_Copy(bitmap, left, 0, left + size, size);
        setTile(tile, group.x, group.y);

        left += size;
    }
}

FIBITMAP* Tileset::getBitmap() {
    return bitmap;
}

Tileset::Tileset(FIBITMAP* bitmap, unsigned int size) {
    this->bitmap = bitmap;
    this->size = size;
}