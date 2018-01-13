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

namespace diggler {

class CaveGenerator;
class Game;
class World;
using WorldRef = std::shared_ptr<World>;

namespace render {
class WorldRenderer;
}

namespace net {
namespace MsgTypes {
struct BlockUpdateNotify;
}
}

class Chunk {
private:
  friend World;
  friend CaveGenerator;
  friend class render::WorldRenderer;
  uintptr_t rendererData;

public:
  constexpr static int
    CX = 16, /**< Chunk size along X axis, in blocks. */
    CY = 16, /**< Chunk size along Y axis, in blocks. */
    CZ = 16; /**< Chunk size along Z axis, in blocks. */
  constexpr static float CullSphereRadius =
    (CZ > (CX > CY ? CX : CY) ? CZ : (CX > CY ? CX : CY));
    // * 1.4142135623f; but we're already at 2x the radius (i.e. diameter)
  constexpr static float MidX = CX/2.f, MidY = CY/2.f, MidZ = CZ/2.f;

  const int wcx, wcy, wcz;

  /**
   * @brief Struct holding the Chunk's block contents.
   * @note This structure must be a Plain Old Data struct.
   */
  struct Data {
    static constexpr FourCC MagicMarker = MakeFourCC("CKDT");
    FourCC magic; /**< Magic marker. Used for Chunk eviction checking. */

    BlockId id[CX*CY*CZ]; /**< Block IDs. */
    BlockData data[CX*CY*CZ]; /**< Block data. */
    LightData light[CX*CY*CZ]; /**< Block light info. */

    void clear();
  };

  constexpr static int AllocaSize = sizeof(Data);

  enum class State : uint8 {
    Unavailable, /**< The Chunk is unavailable right now. */
    Generating, /**< The Chunk is generating. */
    Loading, /**< The Chunk is loading from storage or network. */
    Ready, /**< The Chunk is loaded properly and ready to use. */
    Evicted /**< The Chunk has been evicted from memory. */
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
    void flush(net::MsgTypes::BlockUpdateNotify&);
    void discard();
  } CH;

  Chunk(Game *G, WorldRef W, int X, int Y, int Z);
  ~Chunk();

  /* ============ Events ============ */

  void onRenderPropertiesChanged();

  /* ============ Getters ============ */

  /**
   * @brief Get the World in which this Chunk resides.
   * @return Reference to belonging World.
   */
  inline WorldRef getWorld() const {
    return W;
  }

  /**
   * @brief Get this Chunk's position in the World.
   * @return Chunk's position in the World.
   */
  inline glm::ivec3 getWorldChunkPos() const {
    return glm::ivec3(wcx, wcy, wcz);
  }

  /**
   * @brief Get the block ID at specified location.
   * @param x X block coordinate within the Chunk.
   * @param y Y block coordinate within the Chunk.
   * @param z Z block coordinate within the Chunk.
   * @return The block ID at specified location.
   */
  BlockId getBlockId(int x, int y, int z);

  /**
   * @brief Get the block data at specified location.
   * @param x X block coordinate within the Chunk.
   * @param y Y block coordinate within the Chunk.
   * @param z Z block coordinate within the Chunk.
   * @return Block's data integer.
   */
  BlockData getBlockData(int x, int y, int z);

  /**
   * @brief Get if the block has extdata.
   * @param x X block coordinate within the Chunk.
   * @param y Y block coordinate within the Chunk.
   * @param z Z block coordinate within the Chunk.
   * @return `true` if block has extdata, `false` otherwise.
   */
  bool blockHasExtdata(int x, int y, int z);

#if 0
  /**
   * @brief Gets a block's extdata.
   * Gets a block's extdata store,  to save advanced state values.
   * @param x
   * @param y
   * @param z
   * @return Block's extdata.
   * @throws NoExtdataOnBlock if the targeted block doesn't have extdata
   */
  goodform::object& getBlockExtdata(int x, int y, int z);
#endif

  bool isDirty() const {
    return dirty;
  }

  /* ============ Setters ============ */

  /**
   * @brief Set the block at specified location, replacing its ID and data.
   * @param x X block coordinate within the Chunk.
   * @param y Y block coordinate within the Chunk.
   * @param z Z block coordinate within the Chunk.
   * @param id Block ID to set.
   * @param data Block data to set.
   */
  void setBlock(int x, int y, int z, BlockId id, BlockData data = 0);

  /**
   * @brief Set the block ID at specified location, keeping its (meta)data.
   * @param x X block coordinate within the Chunk.
   * @param y Y block coordinate within the Chunk.
   * @param z Z block coordinate within the Chunk.
   * @param id Block ID to set.
   */
  void setBlockId(int x, int y, int z, BlockId id);

  /**
   * @brief Set the block data at specified location, keeping its ID.
   * @param x X block coordinate within the Chunk.
   * @param y Y block coordinate within the Chunk.
   * @param z Z block coordinate within the Chunk.
   * @param data Block data to set.
   */
  void setBlockData(int x, int y, int z, BlockData data);

  // Copies extdata tree
  // TODO void setExtdata(const msgpack::object &meta);

  /* ============ Updates ============ */

  void notifyChange(int x, int y, int z);

  /**
   * @brief Marks chunk as dirty.
   * Marks chunk as dirty so it is re-rendered.
   */
  void markAsDirty();

  void updateClient();
  void updateServer();

  /* ============ Serialization ============ */

  void write(io::OutStream&) const;
  void read(io::InStream&);
};

using ChunkRef = std::shared_ptr<Chunk>;
using ChunkWeakRef = std::weak_ptr<Chunk>;

}

#endif
