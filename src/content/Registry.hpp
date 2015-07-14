#ifndef CONTENT_REGISTRY_HPP
#define CONTENT_REGISTRY_HPP
#include "Content.hpp"
#include "../Texture.hpp"
#include "../util/AtlasCreator.hpp"

namespace Diggler {

enum class FaceDirection : uint8_t {
	XInc = 0,
	XDec = 1,
	YInc = 2,
	YDec = 3,
	ZInc = 4,
	ZDec = 5
};

class ContentRegistry {
private:
	// Client
	AtlasCreator *m_atlasCreator;
	const Texture *m_atlas;
	// TODO remove?
	union BlockFaceTexCoords {
		struct {
			AtlasCreator::Coord front, back, top, bottom, left, right;
		};
		AtlasCreator::Coord coords[6];
	};
	std::vector<BlockFaceTexCoords> m_coords;
	AtlasCreator::Coord m_unknownBlockTex;

	// Shared
	;

	// No copy
	ContentRegistry(const ContentRegistry&) = delete;
	ContentRegistry& operator=(const ContentRegistry&) = delete;

public:
	ContentRegistry();
	~ContentRegistry();

	static bool isTransparent(BlockId id);
	static bool isFaceVisible(BlockId id1, BlockId id2);
	static bool canEntityGoThrough(BlockId id/* , Entity& ent*/);
	const AtlasCreator::Coord* gTC(BlockId, FaceDirection) const;
	const Texture* getAtlas() const;

	void registerBlock(/* TODO */);
	void registerItem(/* TODO */);
	void registerFluid(/* TODO */);
	void registerMapgen(/* TODO */);
};

}

#endif