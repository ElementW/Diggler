#ifndef DIGGLER_TEXTURE_PACKER_HPP
#define DIGGLER_TEXTURE_PACKER_HPP

#include "../Platform.hpp"

namespace Diggler {

class Texture;

class TexturePacker {
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
	TexturePacker(const TexturePacker&) = delete;
	TexturePacker& operator=(const TexturePacker&) = delete;

public:
	TexturePacker(uint w, uint h);
	~TexturePacker();

	Coord add(const std::string &path);
	Coord add(int width, int height, int channels, const uint8* data);

	void freezeTexUpdate(bool);
	const Texture* getAtlas();
};

}

#endif /* DIGGLER_TEXTURE_PACKER */
