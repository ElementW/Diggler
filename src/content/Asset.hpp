#ifndef DIGGLER_CONTENT_ASSET_HPP
#define DIGGLER_CONTENT_ASSET_HPP

#include <memory>

#include <meiose/variant.hpp>

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

  Crypto::SHA256::Digest m_hash;

  Type m_type;

  meiose::variant m_metadata;

  Crypto::SHA256::Digest m_contentHash;
  std::shared_ptr<AssetContentMetadata> m_contentMetadata;


  Asset(const std::shared_ptr<AssetContentMetadata>&);

public:
  Type type() const {
    return m_type;
  }

  const Crypto::SHA256::Digest& contentHash() const {
    return m_contentHash;
  }

  const AssetContentMetadata& contentMetadata() const {
    return *m_contentMetadata;
  }
};

}
}

#endif /* DIGGLER_CONTENT_ASSET_HPP */
