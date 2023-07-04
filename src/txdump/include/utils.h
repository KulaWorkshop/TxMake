#pragma once
#include <fstream>
#include <vector>

namespace utils {
	size_t findSignature(std::ifstream& file, const std::vector<unsigned char>& signature);
}