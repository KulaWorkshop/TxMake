#pragma once
#include <vector>

#include "FreeImage.h"
#include "constants.h"

std::pair<FIBITMAP*, FIBITMAP*> createSkybox(const std::vector<FIBITMAP*>& skyboxes);