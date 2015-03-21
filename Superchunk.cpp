#include "Superchunk.hpp"
#include "Game.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "lzfx/lzfx.h"
#include "network/Network.hpp"

using std::fopen; using std::fwrite; using std::fread; using std::fclose;

namespace Diggler {

Superchunk::Superchunk(Game *G) : G(G), c(nullptr),
	chunksX(0), chunksY(0), chunksZ(0) {
}

void Superchunk::free() {
	if (c == nullptr)
		return;
	for(int x = 0; x < chunksX; x++) {
		for(int y = 0; y < chunksY; y++) {
			for(int z = 0; z < chunksZ; z++) {
				if (c[x][y][z]) {
					delete c[x][y][z];
				}
			}
			delete[] c[x][y];
		}
		delete[] c[x];
	}
	delete[] c;
}

void Superchunk::setSize(int x, int y, int z) {
	free();
	chunksX = x; chunksY = y; chunksZ = z;
	c = new Chunk***[chunksX];
	for (int x = 0; x < chunksX; x++) {
		c[x] = new Chunk**[chunksY];
		for (int y = 0; y < chunksY; y++) {
			c[x][y] = new Chunk*[chunksZ];
			for (int z = 0; z < chunksZ; z++) {
				c[x][y][z] = nullptr;
			}
		}
	}
}

Superchunk::~Superchunk() {
	free();
}
 
BlockType Superchunk::get(int x, int y, int z) {
	if (x < 0 || y < 0 || z < 0 || x >= chunksX*CX || y >= chunksY*CY || z >= chunksZ*CZ)
		return BlockType::Air;

	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	x %= CX;
	y %= CY;
	z %= CZ;

	if (cx > chunksX || cy > chunksY || cz > chunksZ || !c[cx][cy][cz])
		return BlockType::Air;
	else
		return c[cx][cy][cz]->get(x, y, z);
}

void Superchunk::set(int x, int y, int z, BlockType type) {
	if (x < 0 || y < 0 || z < 0 || x >= chunksX*CX || y >= chunksY*CY || z >= chunksZ*CZ)
		return;
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	x %= CX;
	y %= CY;
	z %= CZ;

	if(!c[cx][cy][cz])
		c[cx][cy][cz] = new Chunk(cx, cy, cz, G);

	c[cx][cy][cz]->set(x, y, z, type);
}

void Superchunk::set2(int x, int y, int z, BlockType type) {
	if (x < 0 || y < 0 || z < 0 || x >= chunksX*CX || y >= chunksY*CY || z >= chunksZ*CZ)
		return;
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	x %= CX;
	y %= CY;
	z %= CZ;

	if(!c[cx][cy][cz])
		c[cx][cy][cz] = new Chunk(cx, cy, cz, G);

	c[cx][cy][cz]->set2(x, y, z, type);
}

static int i(const float &f) {
	if (f >= 0)
		return (int)f;
	return ((int)f)-1;
}

BlockType Superchunk::get(float x, float y, float z) {
	return get(i(x), i(y), i(z));
}

Chunk* Superchunk::getChunk(int cx, int cy, int cz) {
	if (cx < 0 || cy < 0 || cz < 0 ||
		cx >= chunksX || cy >= chunksY || cz >= chunksZ)
		return nullptr;
	if(!c[cx][cy][cz])
		c[cx][cy][cz] = new Chunk(cx, cy, cz, G);
	return c[cx][cy][cz];
}

void Superchunk::render(const glm::mat4& transform) {
	if (Chunk::R.prog == nullptr)
		return;
	Chunk::R.prog->bind();
	glUniform1f(Chunk::R.uni_fogStart, G->RP->fogStart);
	glUniform1f(Chunk::R.uni_fogEnd, G->RP->fogEnd);
	glUniform1f(Chunk::R.uni_time, G->Time);
	glEnableVertexAttribArray(Chunk::R.att_coord);
	glEnableVertexAttribArray(Chunk::R.att_texcoord);
	glEnableVertexAttribArray(Chunk::R.att_color);
	glEnableVertexAttribArray(Chunk::R.att_wave);
	Chunk::TextureAtlas->bind();

	const static glm::vec3 cShift(Chunk::MidX, Chunk::MidY, Chunk::MidZ);
	glm::mat4 chunkTransform;
	for (int x = 0; x < chunksX; x++)
		for (int y = 0; y < chunksY; y++)
			for (int z = 0; z < chunksZ; z++)
				if (c[x][y][z]) {
					glm::vec3 translate = glm::vec3(x * CX, y * CY, z * CZ);
					if (G->LP->camera.frustum.sphereInFrustum(translate + cShift, Chunk::CullSphereRadius)) {
						chunkTransform = glm::translate(transform, translate);
						c[x][y][z]->renderBatched(chunkTransform);
					}
				}
	
	glDisableVertexAttribArray(Chunk::R.att_wave);
	glDisableVertexAttribArray(Chunk::R.att_color);
	glDisableVertexAttribArray(Chunk::R.att_texcoord);
	glDisableVertexAttribArray(Chunk::R.att_coord);
}

void Superchunk::onRenderPropertiesChanged() {
	c[0][0][0]->onRenderPropertiesChanged();
	refresh();
}

void Superchunk::refresh() {
	for (int x = 0; x < chunksX; x++)
		for (int y = 0; y < chunksY; y++)
			for (int z = 0; z < chunksZ; z++)
				if (c[x][y][z])
					c[x][y][z]->dirty = true;
}

int Superchunk::getChunksX() const {
	return chunksX;
}

int Superchunk::getChunksY() const {
	return chunksY;
}

int Superchunk::getChunksZ() const {
	return chunksZ;
}

struct MapTransferHeader {
	struct {
		int32 x, y, z;
	} ChunkSize;
	struct {
		int32 x, y, z;
	} Chunks;
};

void Superchunk::write(OutStream &msg) const {
	MapTransferHeader mth {
		{CX, CY, CZ},
		{getChunksX(), getChunksY(), getChunksZ()}
	};
	msg.writeData(&mth, sizeof(MapTransferHeader));
	const BlockType *chunkData = nullptr;
	uint compressedSize = CX * CY * CZ;
	byte *compressed = new byte[compressedSize];
	for (int sx=0; sx < getChunksX(); sx++) {
		for (int sy=0; sy < getChunksY(); sy++) {
			for (int sz=0; sz < getChunksZ(); sz++) {
				// Chunk may be uninitialized
				if (c[sx][sy][sz] == nullptr) {
					// Chunk is empty (not initialized), mark as missing
					msg.writeI16(-1);
				} else {
					chunkData = c[sx][sy][sz]->blk;
					compressedSize = CX * CY * CZ;
					lzfx_compress(chunkData, CX*CY*CZ, compressed, &compressedSize);
					msg.writeI16(compressedSize);
					msg.writeData(compressed, compressedSize);
				}
			}
		}
	}
	delete[] compressed;
}

void Superchunk::read(InStream &M) {
	MapTransferHeader mth;
	M.readData(&mth, sizeof(mth));
	setSize(mth.Chunks.x, mth.Chunks.y, mth.Chunks.z);
	int bytesRead = 0;
	uint uncompressedDataSize = CX * CY * CZ; // Should not change
	BlockType *uncompressedData = new BlockType[uncompressedDataSize/sizeof(BlockType)];
	for (int sx=0; sx < mth.Chunks.x; sx++) {
		for (int sy=0; sy < mth.Chunks.y; sy++) {
			for (int sz=0; sz < mth.Chunks.z; sz++) {
				int16 size = M.readI16();
				if (c[sx][sy][sz] != nullptr) {
					delete c[sx][sy][sz]; // Bash out the old chunk
				}
				if (size == -1) { // Chunk is empty
					c[sx][sy][sz] = nullptr; // Keep out
				} else {
					c[sx][sy][sz] = new Chunk(sx, sy, sz, G);
					byte *compressedData = (byte*)malloc(size);
					M.readData(compressedData, size);
					bytesRead += size;
					uncompressedDataSize = CX * CY * CZ;
					lzfx_decompress(compressedData, size, uncompressedData, &uncompressedDataSize);
					for (int i=0; i < CX*CY*CZ; ++i) {
						c[sx][sy][sz]->blk[i] = uncompressedData[i];
					}
					::free(compressedData);
				}
			}
		}
	}
	delete[] uncompressedData;
	getDebugStream() << "MapRead: read " << bytesRead << std::endl;
}

}