#include "constants.h"


std::pair<std::vector<Group>, TilesetDimensions> loadGroups(std::ifstream& inputFile, unsigned int version) {
	TilesetDimensions dimensions = { 14, 4 };

	uint16_t highX = 0;
	uint16_t highY = 0;

	std::vector<Group> groups;

	size_t textureCount = 0;

	if (version == 3) {
		textureCount = 56;
	}
	else if (version == 2) {
		textureCount = 50;
	}
	else {
		textureCount = 27;
	}

	for (unsigned int i = 0; i < textureCount; i++) {
		uint16_t vramX = 0;
		uint16_t vramY = 0;
		inputFile.read((char*)&vramX, 2);
		inputFile.read((char*)&vramY, 2);
		vramX /= 32;
		vramY -= 256;
		vramY /= 64;

		Group group = { vramX, vramY };
		groups.push_back(group);

		inputFile.seekg(4096 + 4, std::ios::cur);

		if (vramX > highX) {
			dimensions.tileWidth = vramX + 1;
			highX = vramX;
		};

		if (vramY > highY) {
			dimensions.tileHeight = vramY + 1;
			highY = vramY;
		}
	}

	return std::make_pair(groups, dimensions);
}