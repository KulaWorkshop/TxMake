#pragma once

#include <iostream>
#include <fstream>

#include "ghc/filesystem.hpp"
namespace fs = ghc::filesystem;

#include "toml.hpp"

struct CONFIG_OPTS {

    struct TEXTURE_PATHS {
        ghc::filesystem::path skybox, tileset64, tileset32, tileset16, tileset8;
    } texturePaths;

    struct PALETTE_PATHS {
        ghc::filesystem::path normal, light, dark;
    };

    PALETTE_PATHS palettePaths;
    PALETTE_PATHS paletteMipPaths;
};

struct RGB_STRENGTH {
	int32_t r;
	int32_t g;
	int32_t b;

	void set(unsigned int def) {
		r += def;
		g += def;
		b += def;
	}
};

struct COLOR_OPTS {
	RGB_STRENGTH modelDark;
	RGB_STRENGTH modelNormal;
	RGB_STRENGTH modelLight;
};

struct COLOR_BLOCK_OPTS {
	RGB_STRENGTH blockUp;
	RGB_STRENGTH blockDown;
	RGB_STRENGTH blockForward;
	RGB_STRENGTH blockBackward;
	RGB_STRENGTH blockLeft;
	RGB_STRENGTH blockRight;
};

int loadConfig(std::fstream& outputFile, fs::path configDirectory, std::string configPath, CONFIG_OPTS& configOpts);