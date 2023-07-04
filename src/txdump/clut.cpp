#include "clut.h"

bool CLUT::fromBuffer(unsigned char* buffer) {
	for (unsigned int i = 0; i < this->size; i++) {
		const uint16_t value = buffer[i * 2] | (buffer[i * 2 + 1] << 8);
		this->colors[i] = color16to32(value);
	}

	return true;
}

CLUT::CLUT(unsigned int size) {
	this->colors = new RGB5[size];
	this->size = size;
}

CLUT::~CLUT() {
	//delete[] this->colors;
}