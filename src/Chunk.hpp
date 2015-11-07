#ifndef CHUNK_HPP
#define CHUNK_HPP
#include <memory>
#include <mutex>
#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include "VBO.hpp"
#include "Program.hpp"
#include "Texture.hpp"
#include "content/Content.hpp"
#include "network/Network.hpp"

#define CX 16
#define CY 16
#define CZ 16

#define CHUNK_INMEM_COMPRESS 1
#define CHUNK_INMEM_COMPRESS_DELAY 2000 /* ms */

namespace Diggler {

class CaveGenerator;
class Game;
class World;
typedef std::shared_ptr<World> WorldRef;

class Chunk {
private:
	friend World;
	friend CaveGenerator;

	static struct Renderer {
		const Program *prog;
		GLint att_coord,
			  att_color,
			  att_texcoord,
			  att_wave,
			  uni_mvp,
			  uni_unicolor,
			  uni_fogStart,
			  uni_fogEnd,
			  uni_time;
	} R;
	static const Texture *TextureAtlas;
	int wcx, wcy, wcz;

	struct Data {
		// Keep me Plain Old Data!
		BlockId id[CX*CY*CZ];
		BlockData data[CX*CY*CZ];
		LightData light[CX*CY*CZ];
		void clear();
	};

public:
	constexpr static int AllocaSize = sizeof(Data);

private:
	Game *G; WorldRef W;
	VBO *vbo, *ibo;
	int vertices, indicesOpq, indicesTpt;

	Data *data, *data2;
	//std::map<int, Datree> metaStore;

	bool dirty;
	std::mutex mut;
	void loadShader();

	void calcMemUsage();
#if CHUNK_INMEM_COMPRESS
	union {
		int imcSize;
		int imcUnusedSince;
	};
	void *imcData;
	void imcCompress();
	void imcUncompress();
#endif

public:
	constexpr static float CullSphereRadius =
		(CZ > (CX > CY ? CX : CY) ? CZ : (CX > CY ? CX : CY));
		// * 1.4142135623f; but we're already at 2x the radius (i.e. diameter)
	constexpr static float MidX = CX/2.f, MidY = CY/2.f, MidZ = CZ/2.f;
	int blkMem;
	enum class State : uint8 {
		Unavailable,
		Generating,
		Loading,
		Ready
	} state;

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
		void flush(Net::OutMessage&);
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
	void setBlock(int x, int y, int z, BlockId id, BlockData data = 0, bool = false);

	///
	/// @brief Sets the block ID at specified location, keeping its (meta)data.
	///
	void setBlockId(int x, int y, int z, BlockId id, bool = false);

	///
	/// @brief Sets the block data at specified location, keeping its ID.
	///
	void setBlockData(int x, int y, int z, BlockData data, bool = false);

	// Copies metadata tree
	// TODO void setMetadata(const Datree &meta);

	/* ============ Updates ============ */

	void notifyChange(int x, int y, int z);

	///
	/// @brief Marks chunk as dirty.
	/// Marks chunk as dirty so it is re-rendered.
	///
	void markAsDirty();

	void updateClient();
	void updateServerPrepare();
	void updateServer();
	void updateServerSwap();

	/* ============ Rendering ============ */

	void render(const glm::mat4 &trasnsform);
	void renderTransparent(const glm::mat4 &transform);

	/* ============ Serialization ============ */

	void write(OutStream&) const;
	void read(InStream&);
	void send(Net::OutMessage&) const;
	void recv(Net::InMessage&);
};

typedef std::shared_ptr<Chunk> ChunkRef;
typedef std::weak_ptr<Chunk> ChunkWeakRef;

}

#endif