#ifndef ATLAS_CREATOR_HPP
#define ATLAS_CREATOR_HPP
#include "../Platform.hpp"

namespace Diggler {

class Texture;

class AtlasCreator {
public:
	struct Coord {
		uint16 x, y, u, v;
	};
	std::vector<Coord> coords;
	int atlasWidth, atlasHeight;

private:
	uint8 *atlasData;

	Texture *atlasTex;
	bool m_freezeTexUpdate;
	void updateTex();

	// No copy
	AtlasCreator(const AtlasCreator&) = delete;
	AtlasCreator& operator=(const AtlasCreator&) = delete;

public:
	AtlasCreator(uint w, uint h);
	~AtlasCreator();

	Coord add(const std::string &path);
	Coord add(int width, int height, int channels, const uint8* data);

	void freezeTexUpdate(bool);
	const Texture* getAtlas();
};

}

#endif