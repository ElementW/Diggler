#include "AssetContentMetadata.hpp"

#include <algorithm>

#include <goodform/variant.hpp>

namespace diggler {
namespace content {

AssetContentMetadata::AssetContentMetadata(AssetManager &am) :
  m_assetManager(am) {
}

AssetContentMetadata::~AssetContentMetadata() {
}

bool AssetContentMetadata::validSchemeVersion(SchemeVersion v) {
  return v == 0;
}

bool AssetContentMetadata::validSerialized(InMemoryStream &ims) {
  try {
    SchemeVersion schemeVersion = ims.readU32();
    if (not validSchemeVersion(schemeVersion)) {
      return false;
    }

    const uint64 fileSize = ims.readU64();
    /* if (fileSize > MaxFileSize) {
      return false;
    } */
    const uint32 blockSize = ims.readU32();
    if (blockSize < MinBlockSize or
        blockSize > MaxBlockSize) {
      return false;
    }
    const uint64 blockCount = (fileSize + (blockSize - 1)) / blockSize;
    ims.seek(static_cast<Stream::OffT>(blockCount * Crypto::SHA256::Digest::Length),
      MemoryStream::Current);
  } catch (const std::underflow_error&) {
    return false;
  }
  return true;
}

bool AssetContentMetadata::validSerializedDB(InMemoryStream &ims) {
  if (not validSerialized(ims)) {
    return false;
  }
  /* Enable when useful:
  Stream::PosT pos = ims.tell();
  ims.rewind();
  const SchemeVersion schemeVersion = ims.readU32();
  ims.seek(pos); */
  try {
    uint8 contentFileStatus = ims.readU8();
    if (contentFileStatus >= static_cast<uint8>(ContentFileStatus::MAX)) {
      return false;
    }
  } catch (const std::underflow_error&) {
    return false;
  }
  return true;
}

void AssetContentMetadata::fromSerialized(InMemoryStream &ims) {
  m_schemeVersion = ims.readU32();
  m_fileSize = ims.readU64();
  m_blockSize = ims.readU32();
  const size_t blockCount = static_cast<size_t>((m_fileSize + (m_blockSize - 1)) / m_blockSize);
  m_blockHashes = decltype(m_blockHashes)(blockCount);
  for (size_t i = 0; i < blockCount; ++i) {
    ims.readData(m_blockHashes[i].data, decltype(m_blockHashes)::value_type::Length);
  }
}

void AssetContentMetadata::fromSerializedDB(InMemoryStream &ims) {
  fromSerialized(ims);
  m_contentFileStatus = static_cast<ContentFileStatus>(ims.readU8());
}

void AssetContentMetadata::toSerialized(OutMemoryStream &oms) const {
  oms.writeU32(m_schemeVersion);
  oms.writeU64(m_fileSize);
  oms.writeU32(m_blockSize);
  for (const Crypto::SHA256::Digest &blockHash : m_blockHashes) {
    oms.writeData(blockHash.data, decltype(m_blockHashes)::value_type::Length);
  }
}

void AssetContentMetadata::toSerializedDB(OutMemoryStream &oms) const {
  toSerialized(oms);
  oms.writeU8(static_cast<uint8>(m_contentFileStatus));
}

}
}
