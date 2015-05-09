#ifndef CHUNK_HPP
#define CHUNK_HPP
#include <mutex>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "VBO.hpp"
#include "Program.hpp"
#include "Texture.hpp"
#include "Blocks.hpp"

#define CX 16
#define CY 16
#define CZ 16

#define CHUNK_INMEM_COMPRESS 1
#define CHUNK_INMEM_COMPRESS_DELAY 2000 /* ms */

namespace Diggler {

class Blocks;
class Game;

class Chunk {
private:
	friend class Superchunk;
	
	BlockType *blk2;
	std::mutex mut;
	void loadShader();
	void set2(int x, int y, int z, BlockType type);

	void calcMemUsage();
#if CHUNK_INMEM_COMPRESS
	void imcCompress();
	void imcUncompress();
#endif

public:
	constexpr static float CullSphereRadius =
		(CZ > (CX > CY ? CX : CY) ? CZ : (CX > CY ? CX : CY));
		// * 1.4142135623f; but we're already at 2x the radius (i.e. diameter)
	constexpr static float MidX = CX/2.f, MidY = CY/2.f, MidZ = CZ/2.f;
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
	static Texture *TextureAtlas;
	BlockType *blk;
#if CHUNK_INMEM_COMPRESS
	union {
		int imcSize;
		int imcUnusedSince;
	};
	uint8 *imcData;
#endif
	int scx, scy, scz;
	int blkMem;
	Game *G;
	VBO *vbo, *ibo;
	int vertices, indicesOpq, indicesTpt;
	bool dirty;
	int lavaCount;

	Chunk(int scx = -1, int scy = -1, int scz = -1, Game *G = nullptr);
	~Chunk();
	void onRenderPropertiesChanged();
	BlockType get(int x, int y, int z);
	void set(int x, int y, int z, BlockType type);
	void updateClient();
	void updateServerPrepare();
	void updateServer();
	void updateServerSwap();

	void render(const glm::mat4 &trasnsform);
	void renderTransparent(const glm::mat4 &transform);
};

}

#endif