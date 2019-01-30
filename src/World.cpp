#include "World.hpp"

#include <cstring>

#include <glm/gtc/matrix_transform.hpp>

#include <minilzo.h>

#include "CaveGenerator.hpp"
#include "Game.hpp"
#include "Universe.hpp"
#include "util/Log.hpp"
#include "world/Emerger.hpp"

namespace diggler {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "World";

World::World(Game *G, WorldId id, bool remote) :
  G(G), id(id), isRemote(remote) {
  // TODO: emerger thread setting, default to std::thread::hardware_concurrency()
  emerger.reset(new world::Emerger);
}

World::~World() {
}


ChunkRef World::getNewEmptyChunk(int cx, int cy, int cz) {
  ChunkRef c = std::make_shared<Chunk>(G, G->U->getWorld(id), cx, cy, cz);
  emplace(std::piecewise_construct, std::tuple<int, int, int>(cx, cy, cz), std::forward_as_tuple(c));
  return c;
}


ChunkRef World::getChunk(int cx, int cy, int cz, bool load) {
  iterator it = find(glm::ivec3(cx, cy, cz));
  if (it != end()) {
    if (!it->second.expired()) {
      return it->second.lock();
    }
  }
  if (load) {
    ChunkRef c = getNewEmptyChunk(cx, cy, cz);
    emerger->queue(c);
    return c;
  }
  return ChunkRef();
}


constexpr auto CX = Chunk::CX, CY = Chunk::CY, CZ = Chunk::CZ;

bool World::setBlock(int x, int y, int z, BlockId id, BlockData data) {
  iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
  if (it != end()) {
    ChunkWeakRef &cwr = it->second;
    ChunkRef cr;
    if ((cr = cwr.lock())) {
      cr->setBlock(rmod(x, CX), rmod(y, CY), rmod(z, CZ), id, data);
      return true;
    }
  }
  return false;
}

bool World::setBlockId(int x, int y, int z, BlockId id) {
  iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
  if (it != end()) {
    ChunkWeakRef &cwr = it->second;
    ChunkRef cr;
    if ((cr = cwr.lock())) {
      cr->setBlockId(rmod(x, CX), rmod(y, CY), rmod(z, CZ), id);
      return true;
    }
  }
  return false;
}

bool World::setBlockData(int x, int y, int z, BlockData data) {
  iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
  if (it != end()) {
    ChunkWeakRef &cwr = it->second;
    ChunkRef cr;
    if ((cr = cwr.lock())) {
      cr->setBlockData(rmod(x, CX), rmod(y, CY), rmod(z, CZ), data);
      return true;
    }
  }
  return false;
}

BlockId World::getBlockId(int x, int y, int z) {
  iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
  if (it != end()) {
    ChunkWeakRef &cwr = it->second;
    ChunkRef cr;
    if ((cr = cwr.lock())) {
      return cr->getBlockId(rmod(x, CX), rmod(y, CY), rmod(z, CZ));
    }
  }
  return content::BlockIgnoreId;
}

BlockData World::getBlockData(int x, int y, int z) {
  iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
  if (it != end()) {
    ChunkWeakRef &cwr = it->second;
    ChunkRef cr;
    if ((cr = cwr.lock())) {
      return cr->getBlockData(rmod(x, CX), rmod(y, CY), rmod(z, CZ));
    }
  }
  return 0;
}

bool World::blockHasExtdata(int x, int y, int z) {
  return getBlockData(x, y, z) & BlockExtdataBit;
}


bool World::raytrace(glm::vec3 pos, glm::vec3 dir, float range, glm::ivec3 *pointed,
  glm::ivec3 *facing) {
  float xPos = floor(pos.x);
  float yPos = floor(pos.y);
  float zPos = floor(pos.z);
  int stepX = signum(dir.x);
  int stepY = signum(dir.y);
  int stepZ = signum(dir.z);
  glm::vec3 ray(dir);
  glm::vec3 tMax(intbound(pos.x, ray.x), intbound(pos.y, ray.y), intbound(pos.z, ray.z));
  glm::vec3 tDelta((float)stepX / ray.x, (float)stepY / ray.y, (float)stepZ / ray.z);
  glm::ivec3 faceDir;
  do {
    BlockId testBlock = getBlockId(xPos, yPos, zPos);
    /// @todo Actual block non-solidity (cursorwise) check
    if (testBlock != content::BlockAirId) {
      if (pointed)
        *pointed = glm::ivec3(xPos, yPos, zPos);
      if (facing)
        *facing = faceDir + glm::ivec3(xPos, yPos, zPos);
      return true;
    }
    if (tMax.x < tMax.y) {
      if (tMax.x < tMax.z) {
        if (tMax.x > range) break;

        xPos += stepX;
        tMax.x += tDelta.x;

        faceDir.x = -stepX;
        faceDir.y = faceDir.z = 0;
      } else {
        if (tMax.z > range) break;
        zPos += stepZ;
        tMax.z += tDelta.z;

        faceDir.x = faceDir.y = 0;
        faceDir.z = -stepZ;
      }
    } else {
      if (tMax.y < tMax.z) {
        if (tMax.y > range) break;
        yPos += stepY;
        tMax.y += tDelta.y;

        faceDir.x = faceDir.z = 0;
        faceDir.y = -stepY;
      } else {
        if (tMax.z > range) break;
        zPos += stepZ;
        tMax.z += tDelta.z;

        faceDir.x = faceDir.y = 0;
        faceDir.z = -stepZ;
      }
    }
  } while (true);
  return false;
}

bool World::raytrace(glm::vec3 pos, glm::vec3 dir, float range, const RayCallback &callback) {
/// @todo Add param to have the callback triggered on either all blocks' traversal OR only on actual hit
  float xPos = floor(pos.x);
  float yPos = floor(pos.y);
  float zPos = floor(pos.z);
  int stepX = signum(dir.x);
  int stepY = signum(dir.y);
  int stepZ = signum(dir.z);
  glm::vec3 ray(dir);
  glm::vec3 tMax(intbound(pos.x, ray.x), intbound(pos.y, ray.y), intbound(pos.z, ray.z));
  glm::vec3 tDelta((float)stepX / ray.x, (float)stepY / ray.y, (float)stepZ / ray.z);
  glm::ivec3 faceDir;
  do {
    BlockId testBlock = getBlockId(xPos, yPos, zPos);
    /// @todo Check for partial blocks
    if (!callback(glm::ivec3(floor(xPos), floor(yPos), floor(zPos)), testBlock,
         glm::vec3(xPos, yPos, zPos), faceDir)) {
      return true;
    }
    if (tMax.x < tMax.y) {
      if (tMax.x < tMax.z) {
        if (tMax.x > range) break;

        xPos += stepX;
        tMax.x += tDelta.x;

        faceDir.x = -stepX;
        faceDir.y = faceDir.z = 0;
      } else {
        if (tMax.z > range) break;
        zPos += stepZ;
        tMax.z += tDelta.z;

        faceDir.x = faceDir.y = 0;
        faceDir.z = -stepZ;
      }
    } else {
      if (tMax.y < tMax.z) {
        if (tMax.y > range) break;
        yPos += stepY;
        tMax.y += tDelta.y;

        faceDir.x = faceDir.z = 0;
        faceDir.y = -stepY;
      } else {
        if (tMax.z > range) break;
        zPos += stepZ;
        tMax.z += tDelta.z;

        faceDir.x = faceDir.y = 0;
        faceDir.z = -stepZ;
      }
    }
  } while (true);
  return false;
}


void World::onRenderPropertiesChanged() {
  ChunkRef c;
  if ((c = begin()->second.lock()))
    c->onRenderPropertiesChanged();
  refresh();
}

void World::refresh() {
  ChunkRef c;
  for (auto pair : *this)
    if ((c = pair.second.lock()))
      c->markAsDirty();
}

void World::write(io::OutStream &msg) const {
  // TODO
  /*
  const void *chunkData = nullptr;
  const uint dataSize = Chunk::AllocaSize;
  uint compressedSize;
  byte *compressed = new byte[dataSize];
  msg.writeU16(size());
  ChunkRef c;
  for (auto pair : *this) {
    if (!(c = pair.second.lock()))
      continue;
    chunkData = c->data;
    compressedSize = dataSize;
    int rz = lzfx_compress(chunkData, dataSize, compressed, &compressedSize);
    const glm::ivec3 &pos = pair.first;
    if (rz < 0) {
      Log(Error, TAG) << "Failed compressing Chunk[" << pos.x << ',' << pos.y <<
          ' ' << pos.z << ']';
    } else {
      msg.writeI16(pos.x);
      msg.writeI16(pos.y);
      msg.writeI16(pos.z);
      msg.writeU16(compressedSize);
      msg.writeData(compressed, compressedSize);
    }
  }
  delete[] compressed;
  */
}

void World::read(io::InStream &M) {
  // TODO
  /*
  int bytesRead = 0;
  uint size = M.readU16();
  for (uint n=0; n < size; ++n) {
    int x = M.readI16(), y = M.readI16(), z = M.readI16();
    uint compressedSize = M.readU16();
    const uint targetDataSize = Chunk::AllocaSize;
    byte *compressedData = new byte[compressedSize];
    M.readData(compressedData, compressedSize);
    bytesRead += compressedSize;
    Chunk &c = *getChunk(x, y, z, true);
    uint outLen = targetDataSize;
    int rz = lzfx_decompress(compressedData, compressedSize, c.data, &outLen);
    if (rz < 0 || outLen != targetDataSize) {
      if (rz < 0) {
        Log(Error, TAG) << "Chunk[" << x << ',' << y << ' ' << z <<
            "] LZFX decompression failed";
      } else {
        Log(Error, TAG) << "Chunk[" << x << ',' << y << ' ' << z <<
            "] has bad size " << outLen << '/' << targetDataSize;
      }
      // TODO: re-request?
    }
    delete[] compressedData;
  }
  Log(Debug, TAG) << "MapRead: read " << bytesRead;
  */
}

}
