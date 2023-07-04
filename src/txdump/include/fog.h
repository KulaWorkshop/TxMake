#pragma once
#include <stdint.h>
#include <iostream>
#include <vector>
#include <fstream>

struct FogGroup {
	size_t palette1;
	size_t palette2;
	size_t palette3;
};

std::vector<FogGroup> getFog(std::ifstream& inputFile, unsigned int version);