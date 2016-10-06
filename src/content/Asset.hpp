#ifndef DIGGLER_CONTENT_ASSET_HPP
#define DIGGLER_CONTENT_ASSET_HPP

#include <goodform/variant.hpp>

#include "../crypto/SHA256.hpp"
#include "AssetContentMetadata.hpp"

namespace Diggler {
namespace Content {

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
  AssetContentMetadata &m_contentMetadata;
  Type m_type;
  goodform::object m_info;

  Asset(AssetContentMetadata&);

public:
  const Crypto::SHA256::Digest& contentHash() const {
    return m_contentHash;
  }

  const AssetContentMetadata& contentMetadata() const {
    return m_contentMetadata;
  }

  Type type() const {
    return m_type;
  }

  const goodform::object& info() const {
    return m_info;
  }
};

}
}

#endif /* DIGGLER_CONTENT_ASSET_HPP */
