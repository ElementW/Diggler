#ifndef CONTENT_REGISTRY_HPP
#define CONTENT_REGISTRY_HPP

#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "BlockDef.hpp"
#include "Content.hpp"
#include "../Texture.hpp"
#include "../util/TexturePacker.hpp"

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
public:
  using BlockIdMap = std::unordered_map<BlockId, BlockDef>;
  using BlockNameMap = std::unordered_map<std::string, BlockIdMap::iterator>;

  class BlockRegistration {
  protected:
    ContentRegistry &registry;
    const BlockNameMap::iterator it;
    enum {
      Uncommitted,
      Committed,
      Moved
    } state;
  public:
    BlockDef &def;

    BlockRegistration(ContentRegistry &registry, const BlockNameMap::iterator &it);
    ~BlockRegistration();

    BlockRegistration(const BlockRegistration&) = delete;
    BlockRegistration& operator=(const BlockRegistration&) = delete;

    BlockRegistration(BlockRegistration&&);
    BlockRegistration& operator=(BlockRegistration&&) = delete;

    BlockId commit();
  };

private:
  friend class Registration;

  // Client
  TexturePacker *m_texturePacker;
  const Texture *m_atlas;
  std::unordered_map<std::string, TexturePacker::Coord> m_textureCoords;

  // Shared
  BlockIdMap m_blocks;
  BlockId m_nextMaxBlockId;
  BlockNameMap m_blockNames;
  std::vector<BlockId> m_freedBlockIds;

  // No copy
  ContentRegistry(const ContentRegistry&) = delete;
  ContentRegistry& operator=(const ContentRegistry&) = delete;

  BlockRegistration registerBlock(BlockId id, const char *name);

public:
  ContentRegistry();
  ~ContentRegistry();

  bool isTransparent(BlockId id) const;
  bool isFaceVisible(BlockId id1, BlockId id2) const;
  bool canEntityGoThrough(BlockId id/* , Entity& ent*/) const;

  TexturePacker::Coord addTexture(const std::string &texName, const std::string &path);
  const TexturePacker::Coord* blockTexCoord(BlockId, FaceDirection, const glm::ivec3&) const;
  const Texture* getAtlas() const;

  BlockRegistration registerBlock(const char *name);
  void registerMapgen(/* TODO */);

  const BlockDef& getBlockDef(BlockId);
};

}

#endif
