#ifndef CHUNK_HPP
#define CHUNK_HPP
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "VBO.hpp"
#include "Program.hpp"
#include "Texture.hpp"
#include "Blocks.hpp"
#include "Mutex.hpp"

#define CX 16
#define CY 16
#define CZ 16

namespace Diggler {

class Blocks;
class Game;

class Chunk /*: Renderable*/ {
private:
	friend class Superchunk;
	
	BlockType *blk2;
	Mutex mut;
	void set2(int x, int y, int z, BlockType type);

public:
	struct GLCoord {
		uint8 x, y, z;
		uint16 tx, ty;
		float r, g, b;
	};
	constexpr static float CullSphereRadius =
		(CZ > (CX > CY ? CX : CY) ? CZ : (CX > CY ? CX : CY));
		// * 1.4142135623f; but we're already at 2x the radius (i.e. diameter)
	constexpr static float MidX = CX/2.f, MidY = CY/2.f, MidZ = CZ/2.f;
	static const Program *RenderProgram;
	static Texture *TextureAtlas;
	static Blocks *BlkInf;
	static GLint RenderProgram_uni_unicolor, RenderProgram_attrib_coord, RenderProgram_attrib_color,
		RenderProgram_attrib_texcoord, RenderProgram_uni_mvp;
	BlockType *blk;
	int scx, scy, scz;
	Game *G;
	VBO *vbo, *ibo;
	int vertices, indices;
	bool changed;
	int lavaCount;
	
	/// @param buffer Wether the chunk is just a buffer chunk
	Chunk(int scx = -1, int scy = -1, int scz = -1, Game *G = nullptr);
	~Chunk();
	BlockType get(int x, int y, int z);
	void set(int x, int y, int z, BlockType type);
	void updateClient();
	void updateServerPrepare();
	void updateServer();
	void updateServerSwap();
	void render(const glm::mat4 &transform);
	void renderBatched(const glm::mat4 &transform);
};

}

#endif