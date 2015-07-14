#ifndef SUPERCHUNK_HPP
#define SUPERCHUNK_HPP
#include "Chunk.hpp"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <glm/detail/type_vec3.hpp>
#include "io/Stream.hpp"
#include "network/Network.hpp"

namespace Diggler {

class Game;
namespace Net {
class InMessage;
class OutMessage;
}

typedef int WorldId;
struct WorldChunkMapSorter {
	constexpr bool operator()(const glm::ivec3& lhs, const glm::ivec3& rhs) const {
		return false;
	}
};
typedef std::map<glm::ivec3, ChunkWeakRef, WorldChunkMapSorter> WorldChunkMap;

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
	std::thread emergerThread;
	std::condition_variable emergerCondVar;
	void emergerProc();

public:
	using WorldChunkMap::size;
	using WorldChunkMap::begin;
	using WorldChunkMap::end;

	int cloudsHeight;
	WorldId id;

	World(Game *G, WorldId id);
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
	ChunkRef getChunkAtCoords(int x, int y, int z);

	///
	/// @brief Gets a chunk.
	/// Gets a chunk of chunk-coordinates `cx, cy, cz`, even if it isn't loaded yet,
	/// and add said chunk to the load queue.
	/// @returns `ChunkRef` to the chunk.
	///
	ChunkRef getChunkEx(int cx, int cy, int cz);
	ChunkRef getChunkExAtCoords(int x, int y, int z);

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

	/* ============ Serialization ============ */

	void write(OutStream&) const;
	void read(InStream&);
	void send(Net::OutMessage&) const;
	void recv(Net::InMessage&);

	/* ============ Rendering ============ */

	void render(const glm::mat4 &transform);
	void renderTransparent(const glm::mat4 &transform);
};

typedef std::shared_ptr<World> WorldRef;
typedef std::weak_ptr<World> WorldWeakRef;

}

#endif