#ifndef CONTENT_REGISTRY_HPP
#define CONTENT_REGISTRY_HPP
#include "Content.hpp"
#include "../Texture.hpp"
#include "../util/TexturePacker.hpp"

namespace Diggler {

class BlockDef;

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
  TexturePacker *m_texturePacker;
  const Texture *m_atlas;
  // TODO remove?
  union BlockFaceTexCoords {
    struct {
      TexturePacker::Coord front, back, top, bottom, left, right;
    };
    TexturePacker::Coord coords[6];
  };
  std::vector<BlockFaceTexCoords> m_coords;
  TexturePacker::Coord m_unknownBlockTex;

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
  const TexturePacker::Coord* gTC(BlockId, FaceDirection) const;
  const Texture* getAtlas() const;

  BlockId registerBlock(/* TODO */);
  BlockId registerItem(/* TODO */);
  BlockId registerFluid(/* TODO */);
  void registerMapgen(/* TODO */);

  const BlockDef& getBlockDef(BlockId);
};

}

#endif
