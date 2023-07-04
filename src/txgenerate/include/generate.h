#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "clut.h"

#include "quantize.h"
#include "segment.h"
#include "tileset.h"

//#include "common.h"

struct GENERATE_OPTS {
	std::string generationType;
	std::string inputImage;
	std::string outputPath;
	bool ditherSkybox = true;
	bool dither64 = true;
	bool dither32 = true;
	bool dither16 = true;
	bool dither8 = true;
	FREE_IMAGE_FILTER scaleMethod32 = FILTER_BICUBIC;
	FREE_IMAGE_FILTER scaleMethod16 = FILTER_BICUBIC;
	FREE_IMAGE_FILTER scaleMethod8 = FILTER_BICUBIC;
};

namespace generate {
	FIBITMAP* skybox(FIBITMAP* image, bool dithering);
	bool tilesets(FIBITMAP* image, ghc::filesystem::path& outputPath, GENERATE_OPTS generateOpts);
}