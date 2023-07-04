#include "config.h"

uint32_t getLightingValue(std::string field) {
	if (field == "dark") return 0;
	else if (field == "normal") return 1;
	else if (field == "light") return 2;
	return 0;
}

void saveModels(std::fstream& outputFile, COLOR_OPTS& opts) {
	outputFile.seekg(24, std::ios::beg);
	outputFile.write((char*)&opts.modelDark.r, 4);
	outputFile.write((char*)&opts.modelDark.g, 4);
	outputFile.write((char*)&opts.modelDark.b, 4);
	outputFile.write((char*)&opts.modelNormal.r, 4);
	outputFile.write((char*)&opts.modelNormal.g, 4);
	outputFile.write((char*)&opts.modelNormal.b, 4);
	outputFile.write((char*)&opts.modelLight.r, 4);
	outputFile.write((char*)&opts.modelLight.g, 4);
	outputFile.write((char*)&opts.modelLight.b, 4);
}

void saveBlocks(std::fstream& outputFile, COLOR_BLOCK_OPTS& opts) {
	outputFile.seekg(0x3C, std::ios::beg);
	outputFile.write((char*)&opts.blockLeft.r, 4);
	outputFile.write((char*)&opts.blockLeft.g, 4);
	outputFile.write((char*)&opts.blockLeft.b, 4);
	outputFile.write((char*)&opts.blockRight.r, 4);
	outputFile.write((char*)&opts.blockRight.g, 4);
	outputFile.write((char*)&opts.blockRight.b, 4);
	outputFile.write((char*)&opts.blockForward.r, 4);
	outputFile.write((char*)&opts.blockForward.g, 4);
	outputFile.write((char*)&opts.blockForward.b, 4);
	outputFile.write((char*)&opts.blockBackward.r, 4);
	outputFile.write((char*)&opts.blockBackward.g, 4);
	outputFile.write((char*)&opts.blockBackward.b, 4);
	outputFile.write((char*)&opts.blockUp.r, 4);
	outputFile.write((char*)&opts.blockUp.g, 4);
	outputFile.write((char*)&opts.blockUp.b, 4);
	outputFile.write((char*)&opts.blockDown.r, 4);
	outputFile.write((char*)&opts.blockDown.g, 4);
	outputFile.write((char*)&opts.blockDown.b, 4);
}

int loadConfig(std::fstream& outputFile, fs::path configDirectory, std::string configPath, CONFIG_OPTS& configOpts) {

    try {

        // Parse config
        const auto table = toml::parse(configPath.c_str());

        // Load model color adjust
        COLOR_OPTS colorOpts;
        const toml::value& adjustConfig  = toml::find(table, "color-adjust");
        const std::vector<uint32_t> modelDark = toml::find<std::vector<uint32_t>>(adjustConfig, "modelDark");
        colorOpts.modelDark.r = modelDark[0];
		colorOpts.modelDark.g = modelDark[1];
		colorOpts.modelDark.b = modelDark[2];
		colorOpts.modelDark.set(4096);

        const std::vector<uint32_t> modelNormal = toml::find<std::vector<uint32_t>>(adjustConfig, "modelNormal");
		colorOpts.modelNormal.r = modelNormal[0];
		colorOpts.modelNormal.g = modelNormal[1];
		colorOpts.modelNormal.b = modelNormal[2];
		colorOpts.modelNormal.set(4096);

		const std::vector<uint32_t> modelLight = toml::find<std::vector<uint32_t>>(adjustConfig, "modelLight");
		colorOpts.modelLight.r = modelLight[0];
		colorOpts.modelLight.g = modelLight[1];
		colorOpts.modelLight.b = modelLight[2];
		colorOpts.modelLight.set(4096);
		saveModels(outputFile, colorOpts);

        // Load block color adjust
        COLOR_BLOCK_OPTS colorBlockOpts;
		const std::vector<uint32_t> blockLeft = toml::find<std::vector<uint32_t>>(adjustConfig, "blockLeft");
		colorBlockOpts.blockLeft.r = blockLeft[0];
		colorBlockOpts.blockLeft.g = blockLeft[1];
		colorBlockOpts.blockLeft.b = blockLeft[2];
		colorBlockOpts.blockLeft.set(128);

		const std::vector<uint32_t> blockRight = toml::find<std::vector<uint32_t>>(adjustConfig, "blockRight");
		colorBlockOpts.blockRight.r = blockRight[0];
		colorBlockOpts.blockRight.g = blockRight[1];
		colorBlockOpts.blockRight.b = blockRight[2];
		colorBlockOpts.blockRight.set(128);

		const std::vector<uint32_t> blockForward = toml::find<std::vector<uint32_t>>(adjustConfig, "blockForward");
		colorBlockOpts.blockForward.r = blockForward[0];
		colorBlockOpts.blockForward.g = blockForward[1];
		colorBlockOpts.blockForward.b = blockForward[2];
		colorBlockOpts.blockForward.set(128);

		const std::vector<uint32_t> blockBackward = toml::find<std::vector<uint32_t>>(adjustConfig, "blockBackward");
		colorBlockOpts.blockBackward.r = blockBackward[0];
		colorBlockOpts.blockBackward.g = blockBackward[1];
		colorBlockOpts.blockBackward.b = blockBackward[2];
		colorBlockOpts.blockBackward.set(128);

		const std::vector<uint32_t> blockUp = toml::find<std::vector<uint32_t>>(adjustConfig, "blockUp");
		colorBlockOpts.blockUp.r = blockUp[0];
		colorBlockOpts.blockUp.g = blockUp[1];
		colorBlockOpts.blockUp.b = blockUp[2];
		colorBlockOpts.blockUp.set(128);

		const std::vector<uint32_t> blockDown = toml::find<std::vector<uint32_t>>(adjustConfig, "blockDown");
		colorBlockOpts.blockDown.r = blockDown[0];
		colorBlockOpts.blockDown.g = blockDown[1];
		colorBlockOpts.blockDown.b = blockDown[2];
		colorBlockOpts.blockDown.set(128);
		saveBlocks(outputFile, colorBlockOpts);

        // Load lighting options
        const toml::value& lightingConfig  = toml::find(table, "lighting");
        uint32_t front = getLightingValue(toml::find<std::string>(lightingConfig, "front"));
        uint32_t back = getLightingValue(toml::find<std::string>(lightingConfig, "back"));
        uint32_t top = getLightingValue(toml::find<std::string>(lightingConfig, "top"));
        uint32_t bottom = getLightingValue(toml::find<std::string>(lightingConfig, "bottom"));
        uint32_t left = getLightingValue(toml::find<std::string>(lightingConfig, "left"));
        uint32_t right = getLightingValue(toml::find<std::string>(lightingConfig, "right"));

        outputFile.seekg(0x12C, std::ios::beg);
        outputFile.write((char*)&top, 4);
        outputFile.write((char*)&right, 4);
        outputFile.write((char*)&front, 4);
        outputFile.write((char*)&back, 4);
        outputFile.write((char*)&left, 4);
        outputFile.write((char*)&bottom, 4);

        // Load tile rotation
        const toml::value& otherConfig  = toml::find(table, "other");
        bool randomTileRotation = toml::find<bool>(otherConfig, "randomTileRotation");
        if (randomTileRotation) {
            outputFile.seekg(0x144, std::ios::beg);
            int temp = 2;
            outputFile.write((char*)&temp, 4);
        }

        // Load texture paths
        const toml::value& textureConfig  = toml::find(table, "textures");
        configOpts.texturePaths.skybox    = configDirectory / toml::find<std::string>(textureConfig, "skybox");
        configOpts.texturePaths.tileset64 = configDirectory / toml::find<std::string>(textureConfig, "tileset-64");
        configOpts.texturePaths.tileset32 = configDirectory / toml::find<std::string>(textureConfig, "tileset-32");
        configOpts.texturePaths.tileset16 = configDirectory / toml::find<std::string>(textureConfig, "tileset-16");
        configOpts.texturePaths.tileset8  = configDirectory / toml::find<std::string>(textureConfig, "tileset-8");

        // Load 64x64 palette paths
        const toml::value& paletteConfig = toml::find(table, "palettes-64");
        configOpts.palettePaths.normal   = configDirectory / toml::find<std::string>(paletteConfig, "normal");
        configOpts.palettePaths.dark     = configDirectory / toml::find<std::string>(paletteConfig, "dark");
        configOpts.palettePaths.light    = configDirectory / toml::find<std::string>(paletteConfig, "light");

        // Load MIP palette paths
        const toml::value& paletteMipConfig = toml::find(table, "palettes-mip");
        configOpts.paletteMipPaths.normal   = configDirectory / toml::find<std::string>(paletteMipConfig, "normal");
        configOpts.paletteMipPaths.dark     = configDirectory / toml::find<std::string>(paletteMipConfig, "dark");
        configOpts.paletteMipPaths.light    = configDirectory / toml::find<std::string>(paletteMipConfig, "light");
    }
    catch (std::exception e) {
		std::string message = e.what();
		std::cerr << "Failed to parse configuration file: " << message << std::endl;
		return 1;
	}

    return 0;
}