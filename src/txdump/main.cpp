#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#define GHC_FILESYSTEM_IMPLEMENTATION
#include "ghc/filesystem.hpp"

#include "FreeImage.h"
#include "constants.h"
#include "utils.h"
#include "clut.h"
#include "texture.h"
#include "skybox.h"
#include "fog.h"

struct THEME_OPTS {
	size_t textureCount, paletteCount;
	unsigned int version;

	THEME_OPTS(size_t textureCount, size_t paletteCount, uint8_t version) : textureCount(textureCount), paletteCount(paletteCount), version(version) {}
} opts(0, 0, 0);

struct DUMP_OPTS {
	bool palettes = false;
	std::string filePath;
	std::string outputPath;
} dumpOpts;

std::vector<Group> groups;
TilesetDimensions tilesetDimensions;

bool parseArguments(int argc, char* argv[]) {
	// Parse flag arguments
	std::vector<std::string> posArgv;
	for (int i = 1; i < argc; i++) {
		std::string argument = argv[i];
		if (argument[0] != '-') {
			posArgv.push_back(argument);
			continue;
		}

		if (argument == "--palettes" || argument == "-p")
			dumpOpts.palettes = true;
		else {
			std::cerr << "Unknown argument: " << argument << std::endl;
			return false;
		}
	}

	// Parse positional arguments
	dumpOpts.filePath = posArgv[0];
	dumpOpts.outputPath = posArgv.size() == 1 ? dumpOpts.filePath + "-dump" : posArgv[1];
	return true;
}

std::vector<FIBITMAP*> readTilesets(std::ifstream& inputFile, size_t textureOffset, std::vector<CLUT>& clutList, int size) {

	std::vector<FIBITMAP*> tilesets;

	int bufferSize = size * size;
	if (size < 64) {
		bufferSize /= 2;
	}

	for (unsigned int b = 0; b < 3; b++) {
		FIBITMAP* tilesetImage = FreeImage_Allocate(size * tilesetDimensions.tileWidth, size * tilesetDimensions.tileHeight, 32);

		inputFile.seekg(textureOffset, std::ios::beg);
		for (unsigned int i = 0; i < opts.textureCount; i++) {
			unsigned char* buffer = (unsigned char*)malloc(bufferSize);
			inputFile.seekg(8, std::ios::cur);
			inputFile.read((char*)buffer, bufferSize);

			Texture texture(size, size, size == 64 ? BPP::BPP8 : BPP::BPP4);

			CLUT clut(16);
			if (opts.version == 3) clut = clutList[constants::paletteDB3[i] * 3 + b];
			else if (opts.version == 2) clut = clutList[constants::paletteDB2[i] * 3 + b];
			else clut = clutList[constants::paletteDB1[i] * 3 + b];

			FIBITMAP* textureBitmap = texture.toBitmap(clut, buffer);
			FreeImage_Paste(tilesetImage, textureBitmap, groups[i].x * size, groups[i].y * size, 256);

			free(buffer);
			FreeImage_Unload(textureBitmap);
		}

		tilesets.push_back(tilesetImage);
	}

	return tilesets;
}

void saveImage(FIBITMAP* image, ghc::filesystem::path path) {
	std::cout << "Writing " << path.string() << std::endl;
	FreeImage_Save(FIF_PNG, image, path.string().c_str());
}

void clut2img(std::vector<CLUT> clutList, unsigned int size, unsigned int start, ghc::filesystem::path path) {
	FIBITMAP* image = FreeImage_Allocate(size, 24, 24);

	for (int i = start; i < 24 * 3; i += 3) {
		CLUT clut = clutList[i];
		for (int c = 0; c < clut.size; c++) {
			RGB5 color = clut.colors[c];
			RGBQUAD colorConv = { color.b, color.g, color.r };
			FreeImage_SetPixelColor(image, c, i / 3, &colorConv);
		}
	}

	FreeImage_FlipVertical(image);
	saveImage(image, path);
	FreeImage_Unload(image);
}

int main(int argc, char* argv[]) {
	// Display banner
	std::cout << "TXDUMP version 1.0-beta.1 - By SaturnKai" << std::endl;
	std::cout << "FreeImage version " << FreeImage_GetVersion() << "\n" << std::endl;

	// Check arguments
	if (argc < 2 || argc > 4) {
		std::cout << "txdump <in-file> [out-directory] [options]" << std::endl;
		std::cout << "  <in-file>       - Path to .TGI source file" << std::endl;
		std::cout << "  [out-directory] - Path for dumped contents (default: *-dump)\n" << std::endl;
		std::cout << "Options:\n  -p, --palettes  - Dump palettes (default: off)\n" << std::endl;
		std::cout << "For more information, please read README.TXT." << std::endl;
		return 0;
	}

	if (!parseArguments(argc, argv))
		return 1;

	FreeImage_Initialise();

	// Open texture file
	std::ifstream inputFile(dumpOpts.filePath, std::ios::binary);
	if (!inputFile.is_open()) {
		std::cerr << "Failed to open texture file: " << argv[1] << std::endl;
		FreeImage_DeInitialise();
		return 1;
	}

	// Setup output directory
	ghc::filesystem::path outputPath(dumpOpts.outputPath);
	if (!ghc::filesystem::exists(outputPath)) {
		if (!ghc::filesystem::create_directories(outputPath)) {
			std::cerr << "Failed to create directory: " << outputPath.string() << std::endl;
			FreeImage_DeInitialise();
			return 1;
		}

		std::cout << "Successfully created directory: " << outputPath.string() << std::endl;
	}
	else std::cout << "Using existing directory: " << outputPath.string() << std::endl;

	// Get version
	uint32_t flag = 0;
	inputFile.seekg(0x164, std::ios::beg);
	inputFile.read((char*)&flag, 4);
	if (flag == 7128) opts = THEME_OPTS(56, 72, 3);
	else if (flag == 2646) opts = THEME_OPTS(50, 54, 2);
	else opts = THEME_OPTS(27, 36, 1);
	std::cout << "Texture file version: " << opts.version << "\n" << std::endl;

	// Get offsets
	const size_t skyboxPaletteOffset = utils::findSignature(inputFile, constants::skyboxPaletteSignature);
	const size_t skyboxTextureOffset = utils::findSignature(inputFile, constants::skyboxTextureSignature);
	if (skyboxPaletteOffset == -1 || skyboxTextureOffset == -1) {
		std::cerr << "Failed to locate skybox signatures!" << std::endl;
		inputFile.close();
		FreeImage_DeInitialise();
		return 1;
	}

	// Get skybox palettes
	inputFile.seekg(skyboxPaletteOffset, std::ios::beg);
	std::vector<CLUT> skyboxClutList;
	for (unsigned int i = 0; i < 24; i++) {
		inputFile.seekg(8, std::ios::cur);
		unsigned char* buffer = (unsigned char*)malloc(256 * 2);
		inputFile.read((char*)buffer, 256 * 2);

		CLUT clut(256);
		clut.fromBuffer(buffer);
		skyboxClutList.push_back(clut);
		free(buffer);
	}

	// Get skybox textures
	std::vector<FIBITMAP*> skyboxes;
	for (unsigned int i = 0; i < 24; i++) {
		inputFile.seekg(skyboxTextureOffset + 8, std::ios::beg);
		unsigned char* buffer = (unsigned char*)malloc(768 * 256);
		inputFile.read((char*)buffer, 768 * 256);

		Texture texture(768, 256, BPP::BPP8);
		FIBITMAP* skybox = texture.toBitmap(skyboxClutList[i], buffer);
		skyboxes.push_back(skybox);

		free(buffer);
	}

	// Save skyboxes
	std::pair<FIBITMAP*, FIBITMAP*> skybox = createSkybox(skyboxes);
	saveImage(skybox.first, outputPath / "skybox.png");
	saveImage(skybox.second, outputPath / "skybox-raw.png");

	// Cleanup
	FreeImage_Unload(skybox.first);
	FreeImage_Unload(skybox.second);
	for (FIBITMAP* s : skyboxes) {
		FreeImage_Unload(s);
	}

	// Get fog
	std::vector<FogGroup> fogGroup = getFog(inputFile, opts.version);

	size_t paletteOffset = 0;
	if (opts.version == 3) paletteOffset = utils::findSignature(inputFile, constants::paletteSignature3) + 8;
	else if (opts.version == 2) paletteOffset = utils::findSignature(inputFile, constants::paletteSignature2) + 8;
	else paletteOffset = utils::findSignature(inputFile, constants::paletteSignature1) + 8;
	size_t textureOffset = utils::findSignature(inputFile, constants::textureSignature);

	if (paletteOffset == -1 || textureOffset == -1) {
		std::cerr << "Failed to locate tileset signatures!" << std::endl;
		inputFile.close();
		FreeImage_DeInitialise();
		return 1;
	}

	// Load groups
	inputFile.seekg(textureOffset, std::ios::beg);
	std::pair<std::vector<Group>, TilesetDimensions> result = loadGroups(inputFile, opts.version);
	groups = result.first;
	tilesetDimensions = result.second;

	inputFile.seekg(paletteOffset, std::ios::beg);
	std::vector<CLUT> clutList;
	for (unsigned int i = 0; i < opts.paletteCount; i++) {
		unsigned char* buffer = (unsigned char*)malloc(256 * 2);
		inputFile.read((char*)buffer, 256 * 2);

		CLUT clut(256);
		clut.fromBuffer(buffer);
		clutList.push_back(clut);
		free(buffer);
	}

	// Save palettes as images
	if (dumpOpts.palettes) {
		clut2img(clutList, 256, 0, outputPath / "palette-64.dark.png");
		clut2img(clutList, 256, 1, outputPath / "palette-64.png");
		clut2img(clutList, 256, 2, outputPath / "palette-64.light.png");
	}

	// Read Mip Palettes
	std::vector<CLUT> mipClutList;
	inputFile.seekg(8, std::ios::cur);
	for (unsigned int i = 0; i < opts.paletteCount / 3; i++) {
		FogGroup group = fogGroup[i];
		for (unsigned int j = 0; j < 3; j++) {

			if (j == 1) {
				inputFile.seekg(32 * group.palette1, std::ios::cur);
			}
			else if (j == 2) {
				inputFile.seekg(32 * group.palette2, std::ios::cur);
			}

			unsigned char* buffer = (unsigned char*)malloc(16 * 2);
			inputFile.read((char*)buffer, 16 * 2);

			CLUT clut(16);
			clut.fromBuffer(buffer);
			mipClutList.push_back(clut);
			free(buffer);
		}

		inputFile.seekg(32 * group.palette3, std::ios::cur);
	}

	// Save palettes as images
	if (dumpOpts.palettes) {
		clut2img(mipClutList, 16, 0, outputPath / "palette-mip.dark.png");
		clut2img(mipClutList, 16, 1, outputPath / "palette-mip.png");
		clut2img(mipClutList, 16, 2, outputPath / "palette-mip.light.png");
	}

	// Save tilesets
	for (unsigned int i = 64; i >= 8; i /= 2) {
		std::vector<FIBITMAP*> tilesets = readTilesets(inputFile, i == 64 ? (std::streampos)textureOffset : inputFile.tellg(), i == 64 ? clutList : mipClutList, i);
		saveImage(tilesets[0], outputPath / ("tileset-" + std::to_string(i) + ".dark.png"));
		saveImage(tilesets[1], outputPath / ("tileset-" + std::to_string(i) + ".png"));
		saveImage(tilesets[2], outputPath / ("tileset-" + std::to_string(i) + ".light.png"));
		for (FIBITMAP* t : tilesets) FreeImage_Unload(t);
	}

	// Cleanup
	inputFile.close();
	FreeImage_DeInitialise();
	return 0;
}