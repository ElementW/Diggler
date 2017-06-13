#ifndef DIGGLER_WORLD_HPP
#define DIGGLER_WORLD_HPP

#include "Chunk.hpp"

#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

#include <glm/detail/type_vec3.hpp>

#include "io/Stream.hpp"
#include "network/Network.hpp"
#include "Particles.hpp"

namespace Diggler {

class Game;
namespace Net {
class InMessage;
class OutMessage;
}

using WorldId = int;
struct WorldChunkMapSorter {
  constexpr bool operator()(const glm::ivec3 &lhs, const glm::ivec3 &rhs) const {
    if (lhs.x == rhs.x) {
      if (lhs.y == rhs.y) {
        if (lhs.z == rhs.z) {
          return false; // lhs == rhs isn't lhs < rhs
        } else {
          return (lhs.z < rhs.z);
        }
      } else {
        return (lhs.y < rhs.y);
      }
    }
    return (lhs.x < rhs.x);
  }
};
using WorldChunkMap = std::map<glm::ivec3, ChunkWeakRef, WorldChunkMapSorter>;

class World final : private WorldChunkMap {
private:
  friend class Chunk;
  friend class CaveGenerator;

  Game *G;

  std::queue<ChunkWeakRef> emergeQueue;
  std::mutex emergeQueueMutex;
  void addToEmergeQueue(ChunkRef&);
  void addToEmergeQueue(ChunkWeakRef&);
  bool emergerRun;
  std::vector<std::thread> emergerThreads;
  std::condition_variable emergerCondVar;
  void emergerProc(int);

public:
  using WorldChunkMap::size;
  using WorldChunkMap::begin;
  using WorldChunkMap::end;

  std::vector<ParticleEmitter> emitters;

  const WorldId id;
  const bool isRemote;

  World(Game *G, WorldId id, bool remote);
  ~World();

  /* ============ Getters ============ */

  ChunkRef getNewEmptyChunk(int cx, int cy, int cz);

  /**
   * @brief Gets a chunk.
   * Gets the chunk at chunk-coordinates @p cx, @p cy, @p cz.
   * Returns an empty reference if the chunk isn't ready and @p load is `false`. In such cases,
   * the chunk is added to the load queue.
   * @param x Chunk coordinate X axis.
   * @param y Chunk coordinate Y axis.
   * @param z Chunk coordinate Z axis.
   * @param load If the chunk must be loaded.
   * @returns `ChunkRef` to the chunk.
   */
  ChunkRef getChunk(int cx, int cy, int cz, bool load = false);
  inline ChunkRef getChunkAtCoords(int x, int y, int z, bool load = false) {
    return getChunk(divrd(x, Chunk::CX), divrd(y, Chunk::CY), divrd(z, Chunk::CZ), load);
  }
  inline ChunkRef getChunkAtCoords(const glm::ivec3 &v, bool load = false) {
    return getChunk(divrd(v.x, Chunk::CX), divrd(v.y, Chunk::CY), divrd(v.z, Chunk::CZ), load);
  }

  /**
   * @returns The block ID at specified location.
   */
  BlockId getBlockId(int x, int y, int z);

  /**
   * @returns Block's data integer.
   * @note If the block has extdata, 0 is returned.
   */
  BlockData getBlockData(int x, int y, int z);

  /**
   * @returns `true` if block has extdata, `false` otherwise.
   */
  bool blockHasExtdata(int x, int y, int z);

  /**
   * @brief Gets a block's extdata.
   * Gets a block's extdata store, used to save advanced state values.
   * @returns Block's extdata.
   */
  // TODO msgpack::object&& getBlockExtdata(int x, int y, int z);

  /* ============ Setters ============ */

  /**
   * @brief Sets the block at specified location, replacing its ID and data.
   */
  bool setBlock(int x, int y, int z, BlockId id, BlockData data = 0);

  /**
   * @brief Sets the block ID at specified location, keeping its (meta)data.read()
   */
  bool setBlockId(int x, int y, int z, BlockId id);

  /**
   * @brief Sets the block data at specified location, keeping its ID.
   */
  bool setBlockData(int x, int y, int z, BlockData data);

  // Copies extdata tree
  // TODO void setExtdata(const msgpack::object& &meta);

  void onRenderPropertiesChanged();
  void refresh();

  /* ============ Ray tracing ============ */

  using RayCallback = std::function<bool /*continue*/ (
    glm::ivec3 blockPos,
    BlockId blockId,
    glm::vec3 hitPoint,
    glm::vec3 hitNormal)>;

  /**
   * @brief Fires a ray through the voxels and determines what block and facing block is hit.
   * @todo Documentation
   */
  bool raytrace(glm::vec3 pos, glm::vec3 dir, float range, glm::ivec3 *pointed, glm::ivec3 *norm);
  bool raytrace(glm::vec3 from, glm::vec3 to, glm::ivec3 *pointed, glm::ivec3 *facing) {
    return raytrace(from, to-from, glm::length(to-from), pointed, facing);
  }

  bool raytrace(glm::vec3 pos, glm::vec3 dir, float range, const RayCallback &callback);
  bool raytrace(glm::vec3 from, glm::vec3 to, const RayCallback &callback) {
    return raytrace(from, to-from, glm::length(to-from), callback);
  }

  /* ============ Serialization ============ */

  void write(IO::OutStream&) const;
  void read(IO::InStream&);
  void send(Net::OutMessage&) const;
  void recv(Net::InMessage&);
};

using WorldRef = std::shared_ptr<World>;
using WorldWeakRef = std::weak_ptr<World>;

}

#endif /* DIGGLER_WORLD_HPP */
