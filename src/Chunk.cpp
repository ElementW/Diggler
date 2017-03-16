#include "Chunk.hpp"

#include "Platform.hpp"

#include <cstring>
#include <cstddef>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <lzfx.h>
#include <MurmurHash2.h>

#include "GlobalProperties.hpp"
#include "Game.hpp"
#include "content/Registry.hpp"
#include "network/msgtypes/BlockUpdate.hpp"
#include "render/Renderer.hpp"
#include "util/Log.hpp"

#if CHUNK_INMEM_COMPRESS
  #include <cstdlib>
#endif

#define SHOW_CHUNK_UPDATES 1

namespace Diggler {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "Chunk";

constexpr float Chunk::CullSphereRadius;
constexpr float Chunk::MidX, Chunk::MidY, Chunk::MidZ;

static constexpr int CXY = Chunk::CX*Chunk::CY;
static constexpr int I(int x, int y, int z) {
  return x + y*Chunk::CX + z*CXY;
}

void Chunk::Data::clear() {
  memset(this, 0, AllocaSize);
  magic = MagicMarker;
}

Chunk::ChangeHelper::ChangeHelper(Chunk &C) :
  C(C), enabled(true) {
  m_changes.reserve(8);
}

void Chunk::ChangeHelper::add(int x, int y, int z) {
  if (!enabled)
    return;
  m_changes.emplace_back(x, y, z);
}

using Net::MsgTypes::BlockUpdateNotify;
void Chunk::ChangeHelper::flush(BlockUpdateNotify &bun) {
  for (glm::ivec3 &c : m_changes) {
    bun.updates.emplace_back();
    BlockUpdateNotify::UpdateData &upd = bun.updates.back();
    upd.worldId = C.W->id;
    upd.pos = glm::ivec3(C.wcx * CX + c.x, C.wcy * CY + c.y, C.wcz * CZ + c.z);
    upd.id = C.data->id[I(c.x, c.y, c.z)];
    upd.data = C.data->data[I(c.x, c.y, c.z)];
    upd.light = C.data->light[I(c.x, c.y, c.z)];
    upd.cause = BlockUpdateNotify::UpdateData::Cause::Unspecified;
  }
  m_changes.clear();
}

int Chunk::ChangeHelper::count() const {
  return m_changes.size();
}

bool Chunk::ChangeHelper::empty() const {
  return m_changes.empty();
}

void Chunk::ChangeHelper::discard() {
  m_changes.clear();
}

Chunk::Chunk(Game *G, WorldRef W, int X, int Y, int Z) :
  wcx(X), wcy(Y), wcz(Z),
  G(G), W(W), data(nullptr),
  state(State::Unavailable),
  CH(*this) {
  dirty = true;
  data = new Data;
  data->clear();

#if CHUNK_INMEM_COMPRESS
  imcUnusedSince = 0;
  imcData = nullptr;
#endif
  calcMemUsage();

  if (GlobalProperties::IsClient) {
    G->R->renderers.world->registerChunk(this);
  }
}

Chunk::State Chunk::getState() {
  // TODO: check for CKDT magic marker presence when MADV_FREE gets implemented
  return state;
}

void Chunk::calcMemUsage() {
  blkMem = 0;
#if CHUNK_INMEM_COMPRESS
  if (imcData) {
    blkMem = imcSize;
    return;
  }
#endif
  if (data)
    blkMem += AllocaSize;
}

#if CHUNK_INMEM_COMPRESS
void Chunk::imcCompress() {
  if (mut.try_lock()) {
    uint isize = AllocaSize, osize = isize;
    imcData = std::malloc(osize);
    lzfx_compress(data, isize, imcData, &osize);
    imcData = std::realloc(imcData, osize);
    imcSize = osize;
    delete data;
    data = nullptr;
    calcMemUsage();
    mut.unlock();
  }
}

void Chunk::imcUncompress() {
  if (!imcData) {
    imcUnusedSince = G->TimeMs;
    return;
  }
  mut.lock();
  uint isize = imcSize, osize = AllocaSize;
  imcUnusedSince = G->TimeMs;
  data = new Data;
  lzfx_decompress(imcData, isize, data, &osize);
  std::free(imcData);
  imcData = nullptr;
  calcMemUsage();
  mut.unlock();
}
#endif

void Chunk::onRenderPropertiesChanged() {

}

Chunk::~Chunk() {
  delete data;
#if CHUNK_INMEM_COMPRESS
  std::free(imcData);
#endif
  if (GlobalProperties::IsClient) {
    G->R->renderers.world->unregisterChunk(this);
  }
  // getDebugStream() << W->id << '.' << wcx << ',' << wcy << ',' << wcz << " destruct" << std::endl;
}

void Chunk::notifyChange(int x, int y, int z) {
  if (state != State::Ready)
    return;

  markAsDirty();

  CH.add(x, y, z);
  if (GlobalProperties::IsClient) {
    int u = x==CX-1?1:(x==0)?-1:0,
      v = y==CY-1?1:(y==0)?-1:0,
      w = z==CZ-1?1:(z==0)?-1:0;
    ChunkRef nc;
    if (u && (nc = W->getChunk(wcx+u, wcy, wcz)))
      nc->markAsDirty();
    if (v && (nc = W->getChunk(wcx, wcy+v, wcz)))
      nc->markAsDirty();
    if (w && (nc = W->getChunk(wcx, wcy, wcz+w)))
      nc->markAsDirty();
  }
}

void Chunk::setBlock(int x, int y, int z, BlockId id, BlockData data) {
  if ((x < 0 || y < 0 || z < 0 || x >= CX || y >= CY || z >= CZ) && G)
    return (void)W->setBlock(wcx * CX + x, wcy * CY + y, wcz * CZ + z, id, data);
#if CHUNK_INMEM_COMPRESS
  imcUncompress();
#endif
  this->data->id[I(x,y,z)] = id;
  this->data->data[I(x,y,z)] = data;
  notifyChange(x, y, z);
}

void Chunk::setBlockId(int x, int y, int z, BlockId id) {
  if ((x < 0 || y < 0 || z < 0 || x >= CX || y >= CY || z >= CZ) && W)
    return (void)W->setBlockId(wcx * CX + x, wcy * CY + y, wcz * CZ + z, id);
#if CHUNK_INMEM_COMPRESS
  imcUncompress();
#endif
  this->data->id[I(x,y,z)] = id;
  notifyChange(x, y, z);
}

void Chunk::setBlockData(int x, int y, int z, BlockData data) {
  if ((x < 0 || y < 0 || z < 0 || x >= CX || y >= CY || z >= CZ) && W)
    return (void)W->setBlockData(wcx * CX + x, wcy * CY + y, wcz * CZ + z, data);
#if CHUNK_INMEM_COMPRESS
  imcUncompress();
#endif
  this->data->data[I(x,y,z)] = data;
  notifyChange(x, y, z);
}
 
BlockId Chunk::getBlockId(int x, int y, int z) {
  if ((x < 0 || y < 0 || z < 0 || x >= CX || y >= CY || z >= CZ) && W)
    return W->getBlockId(wcx * CX + x, wcy * CY + y, wcz * CZ + z);
#if CHUNK_INMEM_COMPRESS
  imcUncompress();
#endif
  return this->data->id[I(x,y,z)];
}

BlockData Chunk::getBlockData(int x, int y, int z) {
  if ((x < 0 || y < 0 || z < 0 || x >= CX || y >= CY || z >= CZ) && W)
    return W->getBlockData(wcx * CX + x, wcy * CY + y, wcz * CZ + z);
#if CHUNK_INMEM_COMPRESS
  imcUncompress();
#endif
  BlockData d = this->data->data[I(x,y,z)];
  if (d & BlockExtdataBit) {
    // TODO Implement data in extdata
    return 0;
  }
  return d;
}

bool Chunk::blockHasExtdata(int x, int y, int z) {
  if ((x < 0 || y < 0 || z < 0 || x >= CX || y >= CY || z >= CZ) && W)
    return W->blockHasExtdata(wcx * CX + x, wcy * CY + y, wcz * CZ + z);
#if CHUNK_INMEM_COMPRESS
  imcUncompress();
#endif
  return this->data->data[I(x,y,z)] & BlockExtdataBit;
}

void Chunk::markAsDirty() {
  dirty = true;
}

void Chunk::updateServer() {
  mut.lock();
  for (int x=0; x < CX; x++)
    for (int y=0; y < CY; y++)
      for (int z=0; z < CZ; z++) {
        // TODO
#if 0
        if (data->id[I(x,y,z)] == BlockTypeLava) {
          BlockType under = get(x, y-1, z);
          if (under == BlockType::Air) {
            set2(x, y-1, z, BlockTypeLava);
          } else if (under != BlockTypeLava) {
            if (get(x+1, y, z) == BlockType::Air)
              set2(x+1, y, z, BlockTypeLava);
            if (get(x-1, y, z) == BlockType::Air)
              set2(x-1, y, z, BlockTypeLava);
            if (get(x, y, z+1) == BlockType::Air)
              set2(x, y, z+1, BlockTypeLava);
            if (get(x, y, z-1) == BlockType::Air)
              set2(x, y, z-1, BlockTypeLava);
          }
        }
#endif
      }
  mut.unlock();
}

struct RGB { float r, g, b; };
void Chunk::updateClient() {
#if CHUNK_INMEM_COMPRESS
  imcUncompress();
#endif
  mut.lock();
  Content::Registry &CR = *G->CR;
  Vertex vertex[CX * CY * CZ * 6 /* faces */ * 4 /* vertices */ / 2 /* face removing (HSR) makes a lower vert max */];
  ushort idxOpaque[CX * CY * CZ * 6 /* faces */ * 6 /* indices */ / 2 /* HSR */],
         idxTransp[CX*CY*CZ*6*6/2];
  ushort v = 0, io = 0, it = 0;

  BlockId bt, bn /*BlockNear*/;
  const TexturePacker::Coord *tc;
  for(int8 x = 0; x < CX; x++) {
    for(int8 y = 0; y < CY; y++) {
      for(int8 z = 0; z < CZ; z++) {
        const glm::ivec3 blockPos(x + wcx * CX, y + wcy * CY, z + wcz * CZ);
        bt = data->id[I(x,y,z)];

        // Empty block?
        if (bt == Content::BlockAirId ||
          bt == Content::BlockIgnoreId)
          continue;

#if 0
        BlockType
          /* -X face*/
          bNNZ = get(x-1, y-1, z),
          bNPZ = get(x-1, y+1, z),
          bNZN = get(x-1, y, z-1),
          bNZP = get(x-1, y, z+1),
          /* +X face*/
          bPNZ = get(x+1, y-1, z),
          bPPZ = get(x+1, y+1, z),
          bPZN = get(x+1, y, z-1),
          bPZP = get(x+1, y, z+1),
          /* Top & bottom */
          bZPN = get(x, y+1, z-1),
          bZPP = get(x, y+1, z+1),
          bZNN = get(x, y-1, z-1),
          bZNP = get(x, y+1, z+1);
          
          RGB bl = {.6f, .6f, .6f}, br = {.6f, .6f, .6f},
            tl = {.6f, .6f, .6f}, tr = {.6f, .6f, .6f};
          if (bNZN == BlockTypeLava || bNNZ == BlockTypeLava) { bl.r = 1.6f; bl.g = 1.2f; }
          if (bNNZ == BlockTypeLava || bNZP == BlockTypeLava) { br.r = 1.6f; br.g = 1.2f; }
          if (bNZP == BlockTypeLava || bNPZ == BlockTypeLava) { tr.r = 1.6f; tr.g = 1.2f; }
          if (bNPZ == BlockTypeLava || bNZN == BlockTypeLava) { tl.r = 1.6f; tl.g = 1.2f; }
          vertex[v++] = {x,     y,     z,     tc->x, tc->v, bl.r, bl.g, bl.b};
          vertex[v++] = {x,     y,     z + 1, tc->u, tc->v, br.r, br.g, br.b};
          vertex[v++] = {x,     y + 1, z,     tc->x, tc->y, tl.r, tl.g, tl.b};
          vertex[v++] = {x,     y + 1, z + 1, tc->u, tc->y, tr.r, tr.g, tr.b};
#endif

        GLushort *index; ushort i;
        const bool transp = CR.isTransparent(bt);
        if (transp) {
          index = idxTransp;
          i = it;
        } else {
          index = idxOpaque;
          i = io;
        }

        // View from negative x
        bn = getBlockId(x - 1, y, z);
        if (CR.isFaceVisible(bt, bn)) {
          index[i++] = v; index[i++] = v+1; index[i++] = v+2;
          index[i++] = v+2; index[i++] = v+1; index[i++] = v+3;
          tc = CR.blockTexCoord(bt, FaceDirection::XDec, blockPos);
          vertex[v++] = {x, y,     z,     tc->x, tc->v, .6f, .6f, .6f};
          vertex[v++] = {x, y,     z + 1, tc->u, tc->v, .6f, .6f, .6f};
          vertex[v++] = {x, y + 1, z,     tc->x, tc->y, .6f, .6f, .6f};
          vertex[v++] = {x, y + 1, z + 1, tc->u, tc->y, .6f, .6f, .6f};
        }

        // View from positive x
        bn = getBlockId(x + 1, y, z);
        if (CR.isFaceVisible(bt, bn)) {
          index[i++] = v; index[i++] = v+1; index[i++] = v+2;
          index[i++] = v+2; index[i++] = v+1; index[i++] = v+3;
          tc = CR.blockTexCoord(bt, FaceDirection::XInc, blockPos);
          vertex[v++] = {x + 1, y,     z,     tc->u, tc->v, .6f, .6f, .6f};
          vertex[v++] = {x + 1, y + 1, z,     tc->u, tc->y, .6f, .6f, .6f};
          vertex[v++] = {x + 1, y,     z + 1, tc->x, tc->v, .6f, .6f, .6f};
          vertex[v++] = {x + 1, y + 1, z + 1, tc->x, tc->y, .6f, .6f, .6f};
        }

        // Negative Y
        bn = getBlockId(x, y - 1, z);
        if (CR.isFaceVisible(bt, bn)) {
          index[i++] = v; index[i++] = v+1; index[i++] = v+2;
          index[i++] = v+2; index[i++] = v+1; index[i++] = v+3;
          tc = CR.blockTexCoord(bt, FaceDirection::YDec, blockPos);
          vertex[v++] = {x,     y,     z, tc->u, tc->v, .2f, .2f, .2f};
          vertex[v++] = {x + 1, y,     z, tc->u, tc->y, .2f, .2f, .2f};
          vertex[v++] = {x,     y, z + 1, tc->x, tc->v, .2f, .2f, .2f};
          vertex[v++] = {x + 1, y, z + 1, tc->x, tc->y, .2f, .2f, .2f};
        }

        // Positive Y
        bn = getBlockId(x, y + 1, z);
        if (CR.isFaceVisible(bt, bn)) {
          index[i++] = v; index[i++] = v+1; index[i++] = v+2;
          index[i++] = v+2; index[i++] = v+1; index[i++] = v+3;
          tc = CR.blockTexCoord(bt, FaceDirection::YInc, blockPos);
          vertex[v++] = {x,     y + 1,     z, tc->x, tc->v, .8f, .8f, .8f};
          vertex[v++] = {x,     y + 1, z + 1, tc->u, tc->v, .8f, .8f, .8f};
          vertex[v++] = {x + 1, y + 1,     z, tc->x, tc->y, .8f, .8f, .8f};
          vertex[v++] = {x + 1, y + 1, z + 1, tc->u, tc->y, .8f, .8f, .8f};
        }

        // Negative Z
        bn = getBlockId(x, y, z - 1);
        if (CR.isFaceVisible(bt, bn)) {
          index[i++] = v; index[i++] = v+1; index[i++] = v+2;
          index[i++] = v+2; index[i++] = v+1; index[i++] = v+3;
          tc = CR.blockTexCoord(bt, FaceDirection::ZDec, blockPos);
          vertex[v++] = {x,     y,     z, tc->u, tc->v, .4f, .4f, .4f};
          vertex[v++] = {x,     y + 1, z, tc->u, tc->y, .4f, .4f, .4f};
          vertex[v++] = {x + 1, y,     z, tc->x, tc->v, .4f, .4f, .4f};
          vertex[v++] = {x + 1, y + 1, z, tc->x, tc->y, .4f, .4f, .4f};
        }

        // Positive Z
        bn = getBlockId(x, y, z + 1);
        if (CR.isFaceVisible(bt, bn)) {
          index[i++] = v; index[i++] = v+1; index[i++] = v+2;
          index[i++] = v+2; index[i++] = v+1; index[i++] = v+3;
          tc = CR.blockTexCoord(bt, FaceDirection::ZInc, blockPos);
          vertex[v++] = {x,     y,     z + 1, tc->x, tc->v, .4f, .4f, .4f};
          vertex[v++] = {x + 1, y,     z + 1, tc->u, tc->v, .4f, .4f, .4f};
          vertex[v++] = {x,     y + 1, z + 1, tc->x, tc->y, .4f, .4f, .4f};
          vertex[v++] = {x + 1, y + 1, z + 1, tc->u, tc->y, .4f, .4f, .4f};
        }

        if (transp) {
          it = i;
        } else {
          io = i;
        }
      }
    }
  }

  G->R->renderers.world->updateChunk(this, vertex, v, idxOpaque, io, idxTransp, it);
  dirty = false;
  mut.unlock();
}

void Chunk::write(OutStream &os) const {
  const uint dataSize = Chunk::AllocaSize;
  uint compressedSize;
  byte *compressed = new byte[dataSize];

  const void *chunkData = data;
  compressedSize = dataSize;
  int rz = lzfx_compress(chunkData, dataSize, compressed, &compressedSize);
  if (rz < 0) {
    Log(Error, TAG) << "Failed compressing Chunk[" << wcx << ',' << wcy << ' ' << wcz << ']';
  } else {
    os.writeU16(compressedSize);
    os.writeData(compressed, compressedSize);
  }
  os.writeU32(MurmurHash2(chunkData, dataSize, 0xFA0C778C));

  delete[] compressed;
}

void Chunk::read(InStream &is) {
  uint compressedSize = is.readU16();
  const uint targetDataSize = Chunk::AllocaSize;
  byte *compressedData = new byte[compressedSize];
  is.readData(compressedData, compressedSize);

  uint outLen = targetDataSize;
  int rz = lzfx_decompress(compressedData, compressedSize, data, &outLen);
  if (rz < 0 || outLen != targetDataSize) {
    if (rz < 0) {
      Log(Error, TAG) << "Chunk[" << wcx << ',' << wcy << ' ' << wcz <<
          "] LZFX decompression failed";
    } else {
      Log(Error, TAG) << "Chunk[" << wcx << ',' << wcy << ' ' << wcz <<
          "] has bad size " << outLen << '/' << targetDataSize;
    }
  }
  if (is.readU32() != MurmurHash2(data, outLen, 0xFA0C778C)) {
    Log(Error, TAG) << "Chunk[" << wcx << ',' << wcy << ' ' << wcz <<
        "] decompression gave bad chunk content";
  }

  delete[] compressedData;
  state = State::Ready;

  { ChunkRef nc;
    nc = W->getChunk(wcx+1, wcy, wcz); if (nc) nc->markAsDirty();
    nc = W->getChunk(wcx-1, wcy, wcz); if (nc) nc->markAsDirty();
    nc = W->getChunk(wcx, wcy+1, wcz); if (nc) nc->markAsDirty();
    nc = W->getChunk(wcx, wcy-1, wcz); if (nc) nc->markAsDirty();
    nc = W->getChunk(wcx, wcy, wcz+1); if (nc) nc->markAsDirty();
    nc = W->getChunk(wcx, wcy, wcz-1); if (nc) nc->markAsDirty();
  }
}

}
