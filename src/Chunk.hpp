#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <memory>
#include <mutex>

#include <glm/glm.hpp>

#include "Texture.hpp"
#include "content/Content.hpp"
#include "network/Network.hpp"

#define CHUNK_INMEM_COMPRESS 1
#define CHUNK_INMEM_COMPRESS_DELAY 2000 /* ms */

namespace Diggler {

class CaveGenerator;
class Game;
class World;
using WorldRef = std::shared_ptr<World>;

namespace Render {
class WorldRenderer;
}

namespace Net {
namespace MsgTypes {
struct BlockUpdateNotify;
}
}


constexpr int CX = 16, CY = 16, CZ = 16;

class Chunk {
private:
  friend World;
  friend CaveGenerator;
  friend class Render::WorldRenderer;
  uintptr_t rendererData;

public:
  const int wcx, wcy, wcz;

  struct Data {
    static constexpr FourCC MagicMarker = MakeFourCC("CKDT");
    FourCC magic;
    // Keep me Plain Old Data!
    BlockId id[CX*CY*CZ];
    BlockData data[CX*CY*CZ];
    LightData light[CX*CY*CZ];
    void clear();
  };

  constexpr static int AllocaSize = sizeof(Data);

  enum class State : uint8 {
    Unavailable,
    Generating,
    Loading,
    Ready,
    Evicted
  };

  struct Vertex {
    float x, y, z;
    uint16 tx, ty;
    float r, g, b;
  };

  Game *const G;
  const WorldRef W;

private:
  Data *data;
  //std::map<uint16, msgpack::object> extdataStore;

  State state;
  bool dirty;
  std::mutex mut;

public:
  void calcMemUsage();

#if CHUNK_INMEM_COMPRESS
  union {
    uint imcSize;
    uint64 imcUnusedSince;
  };
  void *imcData;
  void imcCompress();
  void imcUncompress();
#endif

  constexpr static float CullSphereRadius =
    (CZ > (CX > CY ? CX : CY) ? CZ : (CX > CY ? CX : CY));
    // * 1.4142135623f; but we're already at 2x the radius (i.e. diameter)
  constexpr static float MidX = CX/2.f, MidY = CY/2.f, MidZ = CZ/2.f;
  uint blkMem;
  State getState();

  class ChangeHelper {
  private:
    Chunk &C;
    std::vector<glm::ivec3> m_changes;

  public:
    bool enabled;

    ChangeHelper(Chunk &C);
    void add(int x, int y, int z);
    bool empty() const;
    int count() const;
    void flush(Net::MsgTypes::BlockUpdateNotify&);
    void discard();
  } CH;

  Chunk(Game *G, WorldRef W, int X, int Y, int Z);
  ~Chunk();

  /* ============ Events ============ */

  void onRenderPropertiesChanged();

  /* ============ Getters ============ */

  inline WorldRef getWorld() const {
    return W;
  }

  inline glm::ivec3 getWorldChunkPos() const {
    return glm::ivec3(wcx, wcy, wcz);
  }

  ///
  /// @returns The block ID at specified location.
  ///
  BlockId getBlockId(int x, int y, int z);

  ///
  /// @returns Block's data integer.
  ///
  BlockData getBlockData(int x, int y, int z);

  ///
  /// @returns `true` if block has extdata, `false` otherwise.
  ///
  bool blockHasExtdata(int x, int y, int z);
  ///
  /// @brief Gets a block's extdata.
  /// Gets a block's extdata store,  to save advanced state values.
  /// @returns Block's extdata.
  /// @throws NoExtdataOnBlock if the targeted block doesn't have extdata
  ///
  // TODO msgpack::object& getBlockExtdata(int x, int y, int z);

  bool isDirty() const {
    return dirty;
  }

  /* ============ Setters ============ */

  ///
  /// @brief Sets the block at specified location, replacing its ID and data.
  ///
  void setBlock(int x, int y, int z, BlockId id, BlockData data = 0);

  ///
  /// @brief Sets the block ID at specified location, keeping its (meta)data.
  ///
  void setBlockId(int x, int y, int z, BlockId id);

  ///
  /// @brief Sets the block data at specified location, keeping its ID.
  ///
  void setBlockData(int x, int y, int z, BlockData data);

  // Copies extdata tree
  // TODO void setExtdata(const msgpack::object &meta);

  /* ============ Updates ============ */

  void notifyChange(int x, int y, int z);

  ///
  /// @brief Marks chunk as dirty.
  /// Marks chunk as dirty so it is re-rendered.
  ///
  void markAsDirty();

  void updateClient();
  void updateServer();

  /* ============ Serialization ============ */

  void write(OutStream&) const;
  void read(InStream&);
};

using ChunkRef = std::shared_ptr<Chunk>;
using ChunkWeakRef = std::weak_ptr<Chunk>;

}

#endif
