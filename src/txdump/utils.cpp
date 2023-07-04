#include "utils.h"

#define CHUNK 1024

size_t utils::findSignature(std::ifstream& file, const std::vector<unsigned char>& signature) {
	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> buffer(CHUNK);

	size_t offset = 0;
	while (file.read((char*)buffer.data(), buffer.size())) {
		for (int i = 0; i < buffer.size() - signature.size() + 1; i++) {
			if (std::equal(signature.begin(), signature.end(), buffer.begin() + i)) {
				return offset + i;
			}
		}
		offset += buffer.size();
	}

	return -1;
}