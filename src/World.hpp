#ifndef SUPERCHUNK_HPP
#define SUPERCHUNK_HPP
#include "Chunk.hpp"
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <glm/detail/type_vec3.hpp>
#include "io/Stream.hpp"
#include "network/Network.hpp"

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

class World : private WorldChunkMap {
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

	const WorldId id;
	const bool isRemote;

	World(Game *G, WorldId id, bool remote);
	~World();

	/* ============ Getters ============ */

	ChunkRef getNewEmptyChunk(int cx, int cy, int cz);

	///
	/// @brief Gets a chunk.
	/// Gets a chunk of chunk-coordinates `cx, cy, cz`. Returns an empty reference
	/// if the chunk isn't ready.
	/// @returns `ChunkRef` to the chunk.
	///
	ChunkRef getChunk(int cx, int cy, int cz);
	inline ChunkRef getChunkAtCoords(int x, int y, int z) {
		return getChunk(divrd(x, CX), divrd(y, CY), divrd(z, CZ));
	}

	///
	/// @brief Gets a chunk.
	/// Gets a chunk of chunk-coordinates `cx, cy, cz`, even if it isn't loaded yet,
	/// and add said chunk to the load queue.
	/// @returns `ChunkRef` to the chunk.
	///
	ChunkRef getChunkEx(int cx, int cy, int cz);
	inline ChunkRef getChunkExAtCoords(int x, int y, int z) {
		return getChunkEx(divrd(x, CX), divrd(y, CY), divrd(z, CZ));
	}

	///
	/// @returns The block ID at specified location.
	///
	BlockId getBlockId(int x, int y, int z, bool = false);

	///
	/// @returns Block's data integer.
	/// @note If the block has metadata, 0 is returned.
	///
	BlockData getBlockData(int x, int y, int z, bool = false);

	///
	/// @returns `true` if block has metadata, `false` otherwise.
	///
	bool blockHasMetadata(int x, int y, int z, bool = false);

	///
	/// @brief Gets a block's metadata.
	/// Gets a block's metadata store, used to save advanced state values.
	/// @returns Block's metadata.
	///
	// TODO Datree& getBlockMetadata(int x, int y, int z);

	/* ============ Setters ============ */

	///
	/// @brief Sets the block at specified location, replacing its ID and data.
	///
	bool setBlock(int x, int y, int z, BlockId id, BlockData data = 0, bool = false);

	///
	/// @brief Sets the block ID at specified location, keeping its (meta)data.read()
	///
	bool setBlockId(int x, int y, int z, BlockId id, bool = false);

	///
	/// @brief Sets the block data at specified location, keeping its ID.
	///
	bool setBlockData(int x, int y, int z, BlockData data, bool = false);

	// Copies metadata tree
	// TODO void setMetadata(const Datree &meta);

	void onRenderPropertiesChanged();
	void refresh();

	/* ============ Ray tracing ============ */

	using RayCallback = std::function<bool /*continue*/ (
		glm::ivec3 blockPos,
		BlockId blockId,
		glm::vec3 hitPoint,
		glm::vec3 hitNormal)>;

	///
	/// @brief Fires a ray through the voxels and determines what block and facing block is hit.
	/// @todo Documentation
	///
	bool raytrace(glm::vec3 pos, glm::vec3 dir, float range, glm::ivec3 *pointed, glm::ivec3 *norm);
	bool raytrace(glm::vec3 from, glm::vec3 to, glm::ivec3 *pointed, glm::ivec3 *facing) {
		return raytrace(from, to-from, glm::length(to-from), pointed, facing);
	}

	bool raytrace(glm::vec3 pos, glm::vec3 dir, float range, const RayCallback &callback);
	bool raytrace(glm::vec3 from, glm::vec3 to, const RayCallback &callback) {
		return raytrace(from, to-from, glm::length(to-from), callback);
	}

	/* ============ Serialization ============ */

	void write(OutStream&) const;
	void read(InStream&);
	void send(Net::OutMessage&) const;
	void recv(Net::InMessage&);

	/* ============ Rendering ============ */

	void render(const glm::mat4 &transform);
	void renderTransparent(const glm::mat4 &transform);
};

using WorldRef = std::shared_ptr<World>;
using WorldWeakRef = std::weak_ptr<World>;

}

#endif
