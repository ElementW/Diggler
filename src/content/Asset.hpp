#ifndef DIGGLER_CONTENT_ASSET_HPP
#define DIGGLER_CONTENT_ASSET_HPP

#include <msgpack.hpp>

#include "../crypto/SHA256.hpp"

namespace Diggler {
namespace Content {

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
  Crypto::SHA256::Digest m_hash;
  Type m_type;
  msgpack::object m_info;

public:
  const Crypto::SHA256::Digest& hash() const {
    return m_hash;
  }

  Type type() const {
    return m_type;
  }

  const msgpack::object& info() const {
    return m_info;
  }
};

}
}

#endif /* DIGGLER_CONTENT_ASSET_HPP */
