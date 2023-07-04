#include "generate.h"

CLUT* img2clut(ghc::filesystem::path path) {
	FIBITMAP* image = FreeImage_Load(FIF_PNG, path.string().c_str());
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

	FreeImage_Unload(image);
	return clutList;
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

FIBITMAP* generate::skybox(FIBITMAP* image, bool dithering) {
	const unsigned int width = FreeImage_GetWidth(image);
	const unsigned int height = FreeImage_GetHeight(image);
	if (width != 775 || height != 384) {
		std::cout << "Warning, recommended skybox size is 775x384! Auto resizing..." << std::endl;
		image = FreeImage_Rescale(image, 775, 384, FILTER_BOX);
	}

	FIBITMAP* skybox = FreeImage_ConvertTo32Bits(image);
	FIBITMAP* skyboxOutput = FreeImage_Allocate(775, 384, 32);

	std::vector<std::vector<Segment>> collections;
	for (unsigned int i = 0; i < 24; i++) {
		std::vector<Segment> collection;

		for (unsigned int j = 0; j < 138; j++) {
			if (segments[j].collection == i) collection.push_back(segments[j]);
		}

		collections.push_back(collection);
	}

	std::vector<FIBITMAP*> rows;
	for (unsigned int i = 0; i < 24; i++) {
		FIBITMAP* cropped = FreeImage_Copy(skybox, 0, i * 16, 775, (i * 16) + 16);
		FIBITMAP* resized = FreeImage_Rescale(cropped, widths[i], 16, FILTER_BOX);

		rows.push_back(resized);
		FreeImage_Unload(cropped);
	}

	for (unsigned int i = 0; i < 24; i++) {
		unsigned int paletteImageWidth = 0;

		for (const Segment& s : segments)
			if (s.palette == i) paletteImageWidth += s.width;

		FIBITMAP* paletteImage = FreeImage_Allocate(paletteImageWidth, 16, 32);

		std::vector<unsigned int> positions;
		std::vector<Segment> col;

		unsigned int x = 0;
		for (const Segment& s : segments) {
			if (s.palette != i) continue;

			FIBITMAP* segmentImage = getSegmentImage(rows, collections[s.collection], s);
			FreeImage_Paste(paletteImage, segmentImage, x, 0, 255);

			positions.push_back(x);
			col.push_back(s);
			x += FreeImage_GetWidth(segmentImage);
			FreeImage_Unload(segmentImage);
		}

		FIBITMAP* result = quantizeImage(paletteImage, 256, dithering);
		unsigned int index = 0;
		for (const unsigned int& x : positions) {
			Segment segment = col[index];

			FIBITMAP* segmentImage = FreeImage_Copy(result, x, 0, x + segment.width, 16);

			unsigned int pos = 0;
			for (const Segment& s : collections[segment.collection]) {
				if (s == segment) break;
				pos += s.width;
			}

			FreeImage_Paste(skyboxOutput, segmentImage, pos, segment.collection * 16, 255);

			index++;
		}

		FreeImage_Unload(result);
		FreeImage_Unload(paletteImage);
	}

	// Cleanup
	FreeImage_Unload(skybox);
	for (FIBITMAP* r : rows)
		FreeImage_Unload(r);

	return skyboxOutput;
}

FIBITMAP* clut2img(CLUT* clutList[24], unsigned int size) {
	FIBITMAP* image = FreeImage_Allocate(size, 24, 24);

	for (int i = 0; i < 24; i++) {
		CLUT* clut = clutList[i];
		int colorIndex = 0;
		for (RGBQUAD color : clut->colors) {
			FreeImage_SetPixelColor(image, colorIndex, i, &color);
			colorIndex++;
		}
	}

	return image;
}

std::pair<FIBITMAP*, CLUT**> gen(FIBITMAP* image, int size, ghc::filesystem::path& outputPath, bool dithering) {
	FIBITMAP* tilesetImage = FreeImage_ConvertTo32Bits(image);
	Tileset* tileset = new Tileset(tilesetImage, size);
	for (int i = 0; i < 24; i++) {
		FIBITMAP* group = tileset->getGroup(i);
		FIBITMAP* result = quantizeImage(group, size == 64 ? 256 : 16, dithering);
		tileset->setGroup(result, i);

		FreeImage_Unload(group);
		FreeImage_Unload(result);
	}

	// Get Palettes
	CLUT** clutList = new CLUT*[24];
	unsigned int index = 23;
	for (int i = 0; i < 24; i++) {
		FIBITMAP* group = tileset->getGroup(i);
		CLUT* clut = new CLUT();
		clut->addBitmap(group);
		clutList[index] = clut;
		index--;
	}

	// Save Tileset
	FIBITMAP* tilesetOut = tileset->getBitmap();
	saveImage(tilesetOut, outputPath / ("tileset-" + std::to_string(size) + ".png"));

	// Save CLUTs
	FIBITMAP* paletteOut = clut2img(clutList, size == 64 ? 256 : 16);

	std::string prefix = (size == 64 ? "64" : "mip");

	saveImage(paletteOut, outputPath / ("palette-" + prefix + ".png"));
	saveImage(paletteOut, outputPath / ("palette-" + prefix + ".light.png"));
	saveImage(paletteOut, outputPath / ("palette-" + prefix + ".dark.png"));

	return std::make_pair(tilesetOut, clutList);
}

FIBITMAP* resize(FIBITMAP* input, int inputSize, int outputSize, FREE_IMAGE_FILTER filter, bool dithering) {
	FIBITMAP* temp = FreeImage_Allocate(outputSize * 14, outputSize * 4, 32);
	Tileset* tileset = new Tileset(input, inputSize);
	Tileset* outputTileset = new Tileset(temp, outputSize);

	for (int i = 0; i < 56; i++) {
		Group group = groups[i];
		FIBITMAP* tile = tileset->getTile(group.x, group.y);
		FIBITMAP* result = FreeImage_Rescale(tile, outputSize, outputSize, filter);
		outputTileset->setTile(result, group.x, group.y);

		FreeImage_Unload(tile);
		FreeImage_Unload(result);
	}

	for (int i = 0; i < 24; i++) {
		FIBITMAP* group = outputTileset->getGroup(i);
		FIBITMAP* result = quantizeImage(group, 16, dithering);
		outputTileset->setGroup(result, i);

		FreeImage_Unload(group);
		FreeImage_Unload(result);
	}

	return outputTileset->getBitmap();
}

FIBITMAP* resizeWP(FIBITMAP* input, CLUT* clutList, int inputSize, int outputSize, FREE_IMAGE_FILTER filter, bool dithering) {
	FIBITMAP* temp = FreeImage_Allocate(outputSize * 14, outputSize * 4, 32);
	Tileset* tileset = new Tileset(input, inputSize);
	Tileset* outputTileset = new Tileset(temp, outputSize);

	// Resize
	for (int i = 0; i < 56; i++) {
		Group group = groups[i];
		FIBITMAP* tile = tileset->getTile(group.x, group.y);
		FIBITMAP* result = FreeImage_Rescale(tile, outputSize, outputSize, filter);
		outputTileset->setTile(result, group.x, group.y);

		FreeImage_Unload(tile);
		FreeImage_Unload(result);
	}

	// Quantize
	for (int i = 0; i < 24; i++) {
		FIBITMAP* group = outputTileset->getGroup(i);
		FIBITMAP* result = quantizeImageWP(group, clutList[i], 16, dithering);
		outputTileset->setGroup(result, i);

		FreeImage_Unload(group);
		FreeImage_Unload(result);
	}

	return outputTileset->getBitmap();
}

bool generate::tilesets(FIBITMAP* image, ghc::filesystem::path& outputPath, GENERATE_OPTS opts) {
	ghc::filesystem::create_directories(outputPath);

	std::pair<FIBITMAP*, CLUT**> result = gen(image, 64, outputPath, opts.dither64);
	FIBITMAP* tileset32Image = resize(result.first, 64, 32, opts.scaleMethod32, opts.dither32);

	result = gen(tileset32Image, 32, outputPath, opts.dither32);
	FIBITMAP* tileset16Image = resizeWP(result.first, img2clut(outputPath / "palette-mip.png"), 32, 16, opts.scaleMethod16, opts.dither16);
	FIBITMAP* tileset8Image = resizeWP(tileset16Image, img2clut(outputPath / "palette-mip.png"), 16, 8, opts.scaleMethod8, opts.dither8);

	saveImage(tileset16Image, outputPath / "tileset-16.png");
	saveImage(tileset8Image, outputPath / "tileset-8.png");

	FreeImage_Unload(tileset32Image);
	FreeImage_Unload(tileset16Image);
	return true;
}