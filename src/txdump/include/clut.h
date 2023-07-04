#pragma once
#include "color.h"

class CLUT {
public:
	unsigned int size;
	RGB5* colors;
	bool fromBuffer(unsigned char* buffer);

	CLUT(unsigned int size);
	~CLUT();
};