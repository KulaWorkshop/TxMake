#include <iostream>

#include <fstream>
#include <vector>

#include "config.h"
#include "FreeImage.h"

#include "segments.h"
#include "clut.h"
#include "tileset.h"
#include "utils.h"

const int SKYBOX_PALETTE_OFFSET = 0x1AFE4;
const int SKYBOX_OFFSET = 0x1E0A4;
const int PALETTE_OFFSET = 0x4E0AC;
const int TILESET_OFFSET = 0x57ABC;
const int PALETTE_SMALL_OFFSET = 0x570B4;
const int TILESET_32_OFFSET = 0x8FC7C;
const int TILESET_16_OFFSET = 0x96E3C;
const int TILESET_8_OFFSET = 0x98BFC;

class RawTexture {
private:
	unsigned int x = 0;
	unsigned int y = 0;
public:
	std::vector<std::vector<int>> indexedPixels;

	RawTexture(unsigned int x, unsigned int y) : x(x), y(y) {
		indexedPixels = std::vector<std::vector<int>>(y, std::vector<int>(x, 0));
	}

	void addImage(std::vector<int> indexed, unsigned int x, unsigned int y, unsigned int width) {
		const unsigned int height = 16;
		for (int arrY = 0; arrY < height; arrY++) {
			for (int arrX = 0; arrX < width; arrX++) {
				const int index = arrY * width + arrX;
				indexedPixels[y + arrY][x + arrX] = indexed[index];
			}
		}
	}

	void exportBuffer(std::fstream& stream) {
		for (int y = 0; y < this->y; y++) {
			for (int x = 0; x < this->x; x++) {
				uint8_t data = this->indexedPixels[y][x];
				stream.write((char*)&data, 1);
			}
		}
	}
};

unsigned char* texture2Buffer(FIBITMAP* texture, CLUT clut, size_t* sizeOut) {
	unsigned int width = FreeImage_GetWidth(texture);
	unsigned int height = FreeImage_GetHeight(texture);

	const size_t size = width * height;
	unsigned char* buffer = (unsigned char*)malloc(size);

	unsigned int i = 0;
	for (int y = height - 1; y >= 0; y--) {
		for (int x = 0; x < width; x++) {
			unsigned int index = 0;
			RGBQUAD color;
			FreeImage_GetPixelColor(texture, x, y, &color);

			for (RGBQUAD c : clut.colors) {
				if (c.rgbRed == color.rgbRed && c.rgbGreen == color.rgbGreen && c.rgbBlue == color.rgbBlue) {
					// printf("%d\n", index);
					break;
				}
				index++;
			}

			buffer[i] = index;
			i++;
		}
	}

	*sizeOut = size;
	return buffer;
}

unsigned char* texture2Buffer32(FIBITMAP* texture, CLUT clut, size_t* sizeOut, Group g) {
	unsigned int width = FreeImage_GetWidth(texture);
	unsigned int height = FreeImage_GetHeight(texture);

	const size_t size = (width / 2) * height;
	unsigned char* buffer = (unsigned char*)malloc(size);

	unsigned int i = 0;
	for (int y = height - 1; y >= 0; y--) {
		for (int x = 0; x < width / 2; x++) {
			unsigned int index1 = 0;
			unsigned int index2 = 0;
			RGBQUAD color1;
			RGBQUAD color2;
			FreeImage_GetPixelColor(texture, x * 2, y, &color1);
			FreeImage_GetPixelColor(texture, (x * 2) + 1, y, &color2);

			for (RGBQUAD c : clut.colors) {
				if (c.rgbRed == color1.rgbRed && c.rgbGreen == color1.rgbGreen && c.rgbBlue == color1.rgbBlue) break;
				index1++;

				if (index1 == clut.colors.size()) printf("hmm\n");
			}

			for (RGBQUAD c : clut.colors) {
				if (c.rgbRed == color2.rgbRed && c.rgbGreen == color2.rgbGreen && c.rgbBlue == color2.rgbBlue) break;
				index2++;
			}

			buffer[i] = (index2 << 4) | index1;
			i++;
		}
	}

	*sizeOut = size;
	return buffer;
}

CLUT* img2clut(FIBITMAP* image) {
	unsigned int size = FreeImage_GetWidth(image);
	CLUT* clutList = new CLUT[24];

	int index = 0;
	for (int i = 0; i < 25; i++) {
		FIBITMAP* part = FreeImage_Copy(image, 0, i, size, 1);
		if (FreeImage_GetWidth(part) == 0) continue;
		CLUT* clut = new CLUT();
		clut->setSize(size);
		clut->fromBitmap(part);
		clutList[index] = *clut;
		index++;
	}

	return clutList;
}

bool createOutput(ghc::filesystem::path outputPath) {
	std::ifstream input("txtemplate.dat", std::ios::binary);
	std::ofstream output(outputPath.string().c_str(), std::ios::binary);

	if (input.is_open() && output.is_open()) {
		output << input.rdbuf();
	}
	else {
		printf("Damn.\n");
		input.close();
		output.close();
		return false;
	}

	input.close();
	output.close();
	return true;
}

CLUT* importTileset(std::string tilesetPath, std::string palettePath, std::string paletteDarkPath, std::string paletteLightPath, std::fstream& outputFile, size_t tilesetSize) {
	FIBITMAP* tilesetImage = FreeImage_Load(FIF_PNG, tilesetPath.c_str());
	FIBITMAP* paletteImage = FreeImage_Load(FIF_PNG, palettePath.c_str());
	FIBITMAP* paletteDarkImage = FreeImage_Load(FIF_PNG, paletteDarkPath.c_str());
	FIBITMAP* paletteLightImage = FreeImage_Load(FIF_PNG, paletteLightPath.c_str());

	Tileset* tileset = new Tileset(tilesetImage, tilesetSize);
	CLUT* clutList = img2clut(paletteImage);
	CLUT* clutDarkList = img2clut(paletteDarkImage);
	CLUT* clutLightList = img2clut(paletteLightImage);

	outputFile.seekp(tilesetSize == 64 ? PALETTE_OFFSET : PALETTE_SMALL_OFFSET, std::ios::beg);
	outputFile.seekp(8, std::ios::cur);

	for (int i = 0; i < 24; i++) {
		size_t size = 0;
		unsigned char* bufferNormal = clutList[i].toBuffer(&size);
		unsigned char* bufferDark = clutDarkList[i].toBuffer(&size);
		unsigned char* bufferLight = clutLightList[i].toBuffer(&size);

		outputFile.write((char*)bufferDark, size);
		outputFile.write((char*)bufferNormal, size);
		outputFile.write((char*)bufferLight, size);

		free(bufferNormal);
		free(bufferDark);
		free(bufferLight);
	}

	outputFile.seekp(tilesetSize == 64 ? TILESET_OFFSET : TILESET_32_OFFSET, std::ios::beg);
	for (int i = 0; i < 56; i++) {
		const Group group = groups[i];
		FIBITMAP* texture = tileset->getTile(group.x, group.y);

		size_t size = 0;
		unsigned char* buffer = nullptr;

		if (tilesetSize == 64) buffer = texture2Buffer(texture, clutList[group.index], &size);
		else buffer = texture2Buffer32(texture, clutList[group.index], &size, group);

		outputFile.seekp(8, std::ios::cur);
		outputFile.write((char*)buffer, size);
		free(buffer);
	}

	delete tileset;
	FreeImage_Unload(tilesetImage);
	FreeImage_Unload(paletteImage);
	FreeImage_Unload(paletteDarkImage);
	FreeImage_Unload(paletteLightImage);

	return clutList;
}

void importTileset16(std::string tilesetPath, CLUT* clutList, std::fstream& outputFile, size_t tilesetSize) {
	FIBITMAP* tilesetImage = FreeImage_Load(FIF_PNG, tilesetPath.c_str());
	Tileset* tileset = new Tileset(tilesetImage, tilesetSize);

	outputFile.seekp(tilesetSize == 16 ? TILESET_16_OFFSET : TILESET_8_OFFSET, std::ios::beg);
	for (int i = 0; i < 56; i++) {
		const Group group = groups[i];
		FIBITMAP* texture = tileset->getTile(group.x, group.y);

		size_t size = 0;
		unsigned char* buffer = texture2Buffer32(texture, clutList[group.index], &size, group);
		outputFile.seekp(8, std::ios::cur);
		outputFile.write((char*)buffer, size);
		free(buffer);
	}

	delete tileset;
	FreeImage_Unload(tilesetImage);
}

FIBITMAP* getSegmentImage(std::vector<FIBITMAP*>& rows, std::vector<Segment>& collection, const Segment& segment) {
	unsigned int width = 0;
	for (unsigned int i = 0; i < collection.size(); i++) {
		if (collection[i] == segment) break;
		width += collection[i].width;
	}

	FIBITMAP* cropped = FreeImage_Copy(rows[segment.collection], width, 0, width + segment.width, 16);
	return cropped;
}

void importSkybox(std::string skyboxPath, std::fstream& outputFile) {
	FIBITMAP* skyboxImage = FreeImage_Load(FIF_PNG, skyboxPath.c_str());

	std::vector<std::vector<Segment>> segmentCollections;
	std::vector<std::vector<Segment>> segmentPalettes;

	for (unsigned int i = 0; i < 24; i++) {
		std::vector<Segment> segmentCollection;
		std::vector<Segment> segmentPalette;

		for (unsigned int j = 0; j < 138; j++) {
			if (segments[j].collection == i) segmentCollection.push_back(segments[j]);
			if (segments[j].palette == i) segmentPalette.push_back(segments[j]);
		}

		segmentCollections.push_back(segmentCollection);
		segmentPalettes.push_back(segmentPalette);
	}

	// Main
	std::vector<FIBITMAP*> rows;
	for (unsigned int i = 0; i < 24; i++) {
		FIBITMAP* cropped = FreeImage_Copy(skyboxImage, 0, i * 16, widths[i], (i * 16) + 16);
		rows.push_back(cropped);
	}

	std::vector<CLUT> clutList;
	std::vector<unsigned char*> buffers;
	for (const std::vector<Segment>& collection : segmentPalettes) {
		CLUT clut;
		clutList.push_back(clut);

		for (Segment s : collection) {
			FIBITMAP* segmentImage = getSegmentImage(rows, segmentCollections[s.collection], s);
			if (!clutList[clutList.size() - 1].addBitmap2(segmentImage, s)) {
				std::cerr << "Warning, skybox palette overflow!" << std::endl;
			}

			size_t size = 0;
			buffers.push_back(texture2Buffer(segmentImage, clutList[clutList.size() - 1], &size));
		}
	}

	RawTexture skybox(768, 256);
	for (const CLUT& c : clutList) {
		for (const Indexed& i : c.indexed) {
			skybox.addImage(i.indexed, i.segment.x, i.segment.y, i.segment.width);
		}
	}

	outputFile.seekp(SKYBOX_OFFSET + 8, std::ios::beg);
	skybox.exportBuffer(outputFile);

	outputFile.seekp(SKYBOX_PALETTE_OFFSET, std::ios::beg);
	for (CLUT& c : clutList) {
		outputFile.seekg(8, std::ios::cur);

		size_t size = 0;
		unsigned char* buffer = c.toBuffer(&size);
		outputFile.write((char*)buffer, size);
	}

	FreeImage_Unload(skyboxImage);
}

int main(int argc, char* argv[]) {

	// Display banner
	std::cout << "TXMAKE version 1.0-beta.1 - By SaturnKai" << std::endl;
	std::cout << "FreeImage version " << FreeImage_GetVersion() << "\n" << std::endl;

	if (argc != 3) {
		std::cout << "txmake <in-config> <out-file>" << std::endl;
		std::cout << "  <in-config> - Path to .TOML configuration file" << std::endl;
		std::cout << "  <out-file>  - Path for created .TGI file\n" << std::endl;
		std::cout << "For more information, please read README.TXT." << std::endl;
		return 0;
	}

	// Create output file
	std::string outputPath = argv[2];
	if (!createOutput(ghc::filesystem::path(outputPath))) {
		std::cerr << "Failed to create output file: " << argv[2] << std::endl;
		return 1;
	}

	// Open output file
	std::fstream outputFile(outputPath, std::ios::binary | std::ios::in | std::ios::out);

	// Load config
	ghc::filesystem::path configPath(argv[1]);
	ghc::filesystem::path configDirectory = configPath.parent_path();

	CONFIG_OPTS configOpts;
	if (loadConfig(outputFile, configDirectory, configPath.string(), configOpts) == 1) {
		std::cerr << "Failed to load config." << std::endl;
		outputFile.close();
		return 1;
	}

	importSkybox(configOpts.texturePaths.skybox.string(), outputFile);
	importTileset(
		configOpts.texturePaths.tileset64.string(),
		configOpts.palettePaths.normal.string(),
		configOpts.palettePaths.dark.string(),
		configOpts.palettePaths.light.string(),
		outputFile, 64);

	CLUT* clutList = importTileset(
		configOpts.texturePaths.tileset32.string(),
		configOpts.paletteMipPaths.normal.string(),
		configOpts.paletteMipPaths.dark.string(),
		configOpts.paletteMipPaths.light.string(),
		outputFile, 32);

	importTileset16(configOpts.texturePaths.tileset16.string(), clutList, outputFile, 16);
	importTileset16(configOpts.texturePaths.tileset8.string(), clutList, outputFile, 8);

	FreeImage_DeInitialise();
	outputFile.close();
}