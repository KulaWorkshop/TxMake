#include "fog.h"

std::vector<FogGroup> getFog(std::ifstream& inputFile, unsigned int version) {

	// Load versioning
	size_t fogOffset = 0;
	size_t fogSize = 0;
	size_t checkAmount = 0;

	if (version == 3) {
		fogOffset = 0x190;
		fogSize = 0x37B0;
		checkAmount = 99;
	}
	else if (version == 2) {
		fogOffset = 0x190;
		fogSize = 0x14AC;
		checkAmount = 49;
	}
	else {
		fogOffset = 0xFC;
		fogSize = 0xDC8;
		checkAmount = 49;
	}

	inputFile.seekg(fogOffset, std::ios::beg);

	uint16_t prevValue = 0;
	inputFile.read((char*)&prevValue, 2);

	unsigned int total = 0;
	unsigned int fog = 1;
	std::vector<FogGroup> groups;
	std::vector<size_t> group;

	for (unsigned int i = 0; i < fogSize / 2; i++) {
		uint16_t value = 0;
		inputFile.read((char*)&value, 2);

		total++;
		if (prevValue != value) {
			if (total == checkAmount) {
				group.push_back(fog - 1);
				if (group.size() == 3) {
					groups.push_back({ group[0], group[1], group[2] });
					group.clear();
				}

				total = 0;
				fog = 0;
			}

			fog++;
			prevValue = value;
		}
	}

	return groups;
}