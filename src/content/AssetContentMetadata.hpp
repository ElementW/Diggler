#ifndef DIGGLER_CONTENT_ASSET_CONTENT_METADATA_HPP
#define DIGGLER_CONTENT_ASSET_CONTENT_METADATA_HPP

#include <vector>

#include "../crypto/SHA256.hpp"
#include "../platform/Types.hpp"

namespace diggler {
namespace content {

class AssetManager;

class AssetContentMetadata {
protected:
  friend class AssetManager;

  uint64 m_fileSize;
  uint32 m_blockSize;
  std::vector<Crypto::SHA256::Digest> m_blockHashes;

public:
  uint64 fileSize() const {
    return m_fileSize;
  }

  uint32 blockSize() const {
    return m_blockSize;
  }

  uint32 blockSize(std::size_t block) const {
    if (block >= m_blockHashes.size()) {
      return 0;
    } else if (block == m_blockHashes.size() - 1) {
      return static_cast<uint32>(
        m_fileSize - (static_cast<uint>(m_blockHashes.size() - 1) * m_blockSize)
      );
    }
    return m_blockSize;
  }

  uint32 blockCount() const {
    // Rounding up
    return static_cast<uint32>((m_fileSize + (m_blockSize - 1)) / m_blockSize);
  }

  const std::vector<Crypto::SHA256::Digest>& blockHashes() const {
    return m_blockHashes;
  }
};

}
}

#endif /* DIGGLER_CONTENT_ASSET_CONTENT_METADATA_HPP */
