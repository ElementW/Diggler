#ifndef SUPERCHUNK_HPP
#define SUPERCHUNK_HPP
#include "Chunk.hpp"


namespace Diggler {

class Game;
namespace Net {
class InMessage;
class OutMessage;
}

class Superchunk {
private:
	friend class Chunk;

	Game *G;
	Chunk ****c;
	int chunksX, chunksY, chunksZ;

	void set2(int x, int y, int z, BlockType type);
	void free();

public:
	Superchunk(Game *G = nullptr);
	~Superchunk();

	void write(OutStream&) const;
	void read(InStream&);

	void setSize(int x, int y, int z);
	int getChunksX() const;
	int getChunksY() const;
	int getChunksZ() const;

	BlockType get(int x, int y, int z);
	BlockType get(float x, float y, float z);
	void set(int x, int y, int z, BlockType type);
	Chunk* getChunk(int cx, int cy, int cz);
	void onRenderPropertiesChanged();
	void refresh();

	void render(const glm::mat4 &transform);
};

}

#endif