#include <iostream>
#include <vector>

#include "generate.h"
#include "utils.h"

#include "common.h"

GENERATE_OPTS generateOpts;

FREE_IMAGE_FILTER parseFilter(std::string& argument) {
	if (argument == "BOX") return FILTER_BOX;
	if (argument == "BICUBIC") return FILTER_BICUBIC;
	if (argument == "BILINEAR") return FILTER_BILINEAR;
	if (argument == "BSPLINE") return FILTER_BSPLINE;
	if (argument == "CATMULLROM") return FILTER_CATMULLROM;
	if (argument == "LANCZOS3") return FILTER_LANCZOS3;

	std::cout << "Unknown scaling method: " << argument << ", using BICUBIC by default." << std::endl;
	return FILTER_BICUBIC;
}

bool parseArguments(int argc, char* argv[]) {
	// Parse flag arguments
	std::vector<std::string> posArgv;
	for (int i = 1; i < argc; i++) {
		std::string argument = argv[i];
		if (argument[0] != '-') {
			posArgv.push_back(argument);
			continue;
		}

		if (argument == "--noditherskybox") generateOpts.ditherSkybox = false;
		else if (argument == "--nodither64") generateOpts.dither64 = false;
		else if (argument == "--nodither32") generateOpts.dither32 = false;
		else if (argument == "--nodither16") generateOpts.dither16 = false;
		else if (argument == "--nodither8") generateOpts.dither8 = false;

		else if (argument == "--scalemethod32") {
			std::string method = argv[i + 1];
			generateOpts.scaleMethod32 = parseFilter(method);
		}
		else if (argument == "--scalemethod16") {
			std::string method = argv[i + 1];
			generateOpts.scaleMethod16 = parseFilter(method);
		}
		else if (argument == "--scalemethod8") {
			std::string method = argv[i + 1];
			generateOpts.scaleMethod8 = parseFilter(method);
		}
		else {
			std::cerr << "Unknown argument: " << argument << std::endl;
			return false;
		}
	}

	// Parse positional arguments
	generateOpts.generationType = posArgv[0];
	generateOpts.inputImage = posArgv[1];
	generateOpts.outputPath = posArgv[2];
	return true;
}

int main(int argc, char* argv[]) {
	// Display banner
	std::cout << "TXGENERATE version 1.0-beta.1 - By SaturnKai" << std::endl;
	std::cout << "FreeImage version " << FreeImage_GetVersion() << "\n" << std::endl;

	if (argc < 4) {
		std::cout << "txgenerate <skybox | tilesets> <in-image> <out-path> [options]" << std::endl;
		std::cout << "  <skybox | tilesets> - Generate skybox or tilesets" << std::endl;
		std::cout << "  <in-image>          - Path to skybox or tileset source image (.PNG)" << std::endl;
		std::cout << "  <out-path>          - Path for generated contents\n" << std::endl;
		std::cout << "Options:\n  --noditherskybox  - Disable dithering when quantizing skybox (default: on)" << std::endl;
		std::cout << "  --nodither64      - Disable dithering when quantizing 64x64  (default: on)" << std::endl;
		std::cout << "  --nodither32      - Disable dithering when quantizing 32x32  (default: on)" << std::endl;
		std::cout << "  --nodither16      - Disable dithering when quantizing 16x16  (default: on)" << std::endl;
		std::cout << "  --nodither8       - Disable dithering when quantizing 8x8    (default: on)" << std::endl;
		std::cout << "\n  --scalemethod32 <method> - Change scaling method when resizing to 32x32 (default: BICUBIC)" << std::endl;
		std::cout << "  --scalemethod16 <method> - Change scaling method when resizing to 16x16 (default: BICUBIC)" << std::endl;
		std::cout << "  --scalemethod8 <method>  - Change scaling method when resizing to 8x8   (default: BICUBIC)\n" << std::endl;
		std::cout << "Available scaling methods: BOX, BICUBIC, BILINEAR, BSPLINE, CATMULLROM, and LANCZOS3." << std::endl;
		std::cout << "For more information, please read README.TXT." << std::endl;
		return 0;
	}

	if (!parseArguments(argc, argv))
		return 1;

	FreeImage_Initialise();
	FIBITMAP* image = FreeImage_Load(FIF_PNG, generateOpts.inputImage.c_str());
	if (image == NULL) {
		std::cerr << "Failed to open source image!" << std::endl;
		FreeImage_DeInitialise();
		return 1;
	}

	std::string operation = argv[1];
	if (generateOpts.generationType == "skybox") {
		FIBITMAP* result = generate::skybox(image, generateOpts.ditherSkybox);
		saveImage(result, generateOpts.outputPath);
		FreeImage_Unload(result);
	}
	else if (generateOpts.generationType == "tilesets") {
		if (FreeImage_GetWidth(image) != 896 || FreeImage_GetHeight(image) != 256) {
			std::cerr << "Tileset image must be 896x256!" << std::endl;
			FreeImage_DeInitialise();
			return 1;
		}

		ghc::filesystem::path outputPath(generateOpts.outputPath);
		generate::tilesets(image, outputPath, generateOpts);
	}
	else {
		std::cerr << "Unknown generation type: " << generateOpts.generationType << std::endl;
	}

	FreeImage_Unload(image);
	FreeImage_DeInitialise();
	return 0;
}