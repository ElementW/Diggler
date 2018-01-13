#ifndef DIGGLER_CONTENT_ASSET_HPP
#define DIGGLER_CONTENT_ASSET_HPP

#include <memory>

#include "../crypto/SHA256.hpp"
#include "AssetContentMetadata.hpp"

namespace diggler {
namespace content {

class AssetManager;

class Asset {
public:
  enum class Type {
    Texture,
    Sound,
    Music,
    Schematic,
    Model,
    VoxelModel
  };

protected:
  friend class AssetManager;

  Crypto::SHA256::Digest m_contentHash;
  std::shared_ptr<AssetContentMetadata> m_contentMetadata;
  Type m_type;

  Asset(const std::shared_ptr<AssetContentMetadata>&);

public:
  const Crypto::SHA256::Digest& contentHash() const {
    return m_contentHash;
  }

  const AssetContentMetadata& contentMetadata() const {
    return *m_contentMetadata;
  }

  Type type() const {
    return m_type;
  }
};

}
}

#endif /* DIGGLER_CONTENT_ASSET_HPP */
