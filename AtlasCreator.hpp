#ifndef ATLAS_CREATOR_HPP
#define ATLAS_CREATOR_HPP
#include "Platform.hpp"
#include "Texture.hpp"

namespace Diggler {

class AtlasCreator {
public:
	struct Coord {
		uint16 x, y, u, v;
	};
	int atlasWidth, atlasHeight,
		unitWidth, unitHeight;
	int lastX, lastY;

private:
	uint8 *atlasData;
	
	// No copy
	AtlasCreator(const AtlasCreator&) = delete;
	AtlasCreator& operator=(const AtlasCreator&) = delete;

public:
	AtlasCreator(int w = 256, int h = 256, int uw = 64, int uh = 64);
	Coord add(const std::string &path);
	Coord add(int width, int height, int channels, const uint8* data);
	Texture* getAtlas();
	~AtlasCreator();
};

}

#endif