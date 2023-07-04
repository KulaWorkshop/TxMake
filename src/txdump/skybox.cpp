#include "skybox.h"

std::pair<FIBITMAP*, FIBITMAP*> createSkybox(const std::vector<FIBITMAP*>& skyboxes) {
	FIBITMAP* skybox = FreeImage_Allocate(775, 384, 24);
	FIBITMAP* rawSkybox = FreeImage_Allocate(775, 384, 32);

	for (unsigned int i = 0; i < 24; i++) {
		unsigned int width = 0;

		std::vector<Segment> filteredSegments;
		for (unsigned int j = 0; j < 138; j++) {
			if (constants::segments[j].collection == i) {
				filteredSegments.push_back(constants::segments[j]);
				width += constants::segments[j].width;
			}
		}

		FIBITMAP* group = FreeImage_Allocate(width, 16, 24);

		unsigned int x = 0;
		for (const Segment& s : filteredSegments) {
			FIBITMAP* crop = FreeImage_Copy(skyboxes[s.palette], s.x, s.y, s.x + s.width, s.y + 16);

			FreeImage_Paste(group, crop, x, 0, 256);
			x += s.width;

			FreeImage_Unload(crop);
		}

		FIBITMAP* resized = FreeImage_Rescale(group, 775, 16, FILTER_BOX);
		FreeImage_Paste(rawSkybox, group, 0, i * 16, 256);
		FreeImage_Paste(skybox, resized, 0, i * 16, 256);

		FreeImage_Unload(resized);
		FreeImage_Unload(group);
	}

	return std::make_pair(skybox, rawSkybox);
}