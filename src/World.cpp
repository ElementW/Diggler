#include "World.hpp"
#include "Universe.hpp"
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <lzfx.h>
#include "Game.hpp"
#include "CaveGenerator.hpp"

namespace Diggler {

World::World(Game *G, WorldId id) :
	G(G), id(id) {
	emergerThreads.emplace_back(&World::emergerProc, this);
}

World::~World() {
	emergerRun = false;
	emergerCondVar.notify_all();
	for (std::thread &t : emergerThreads) {
		if (t.joinable())
			t.join();
	}
	std::unique_lock<std::mutex> lk(emergeQueueMutex);
}

void World::addToEmergeQueue(ChunkRef &cr) {
	{ std::unique_lock<std::mutex> lk(emergeQueueMutex);
		emergeQueue.emplace(cr);
	}
	emergerCondVar.notify_one();
}
void World::addToEmergeQueue(ChunkWeakRef &cwr) {
	{ std::unique_lock<std::mutex> lk(emergeQueueMutex);
		emergeQueue.emplace(cwr);
	}
	emergerCondVar.notify_one();
}

void World::emergerProc() {
	ChunkRef c;
	emergerRun = true;
	while (emergerRun) {
		{ std::unique_lock<std::mutex> lk(emergeQueueMutex);
			if (emergeQueue.size() <= 0) {
				// No more chunks to emerge, wait for more
				emergerCondVar.wait(lk);
				if (!emergerRun)
					break;
			}
			c = emergeQueue.front().lock();
			emergeQueue.pop();
			if (!c) {
				continue; // Chunk was not referenced anymore
			}
		}

		// TODO: loading
		auto genStart = std::chrono::high_resolution_clock::now();
		CaveGenerator::GenConf gc;
		CaveGenerator::Generate(G->U->getWorld(id), gc, c);
		auto genEnd = std::chrono::high_resolution_clock::now();
		auto genDelta = std::chrono::duration_cast<std::chrono::milliseconds>(genEnd - genStart);
		getOutputStream() << "Map gen took " << genDelta.count() << "ms" << std::endl;

		c.reset(); // Release ref ownership
	}
}

ChunkRef World::getNewEmptyChunk(int cx, int cy, int cz) {
	ChunkRef c = std::make_shared<Chunk>(G, G->U->getWorld(id), cx, cy, cz);
	iterator it = emplace(std::piecewise_construct, std::tuple<int, int, int>(cx, cy, cz), std::tuple<ChunkRef>(c)).first;
	return c;
}


ChunkRef World::getChunk(int cx, int cy, int cz) {
	iterator it = find(glm::ivec3(cx, cy, cz));
	if (it != end())
		return it->second.lock();
	return ChunkRef();
}

ChunkRef World::getChunkAtCoords(int x, int y, int z) {
	return getChunk(divrd(x, CX), divrd(y, CY), divrd(z, CZ));
}

ChunkRef World::getChunkEx(int cx, int cy, int cz) {
	iterator it = find(glm::ivec3(cx, cy, cz));
	if (it != end()) {
		if (!it->second.expired())
			return it->second.lock();
	}
	ChunkRef c = getNewEmptyChunk(cx, cy, cz);
	addToEmergeQueue(c);
	return c;
}

ChunkRef World::getChunkExAtCoords(int x, int y, int z) {
	return getChunkEx(divrd(x, CX), divrd(y, CY), divrd(z, CZ));
}


bool World::setBlock(int x, int y, int z, BlockId id, BlockData data, bool buf2) {
	iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
	if (it != end()) {
		ChunkWeakRef &cwr = it->second;
		ChunkRef cr;
		if ((cr = cwr.lock())) {
			cr->setBlock(rmod(x, CX), rmod(y, CY), rmod(z, CZ), id, data, buf2);
			return true;
		}
	}
	return false;
}

bool World::setBlockId(int x, int y, int z, BlockId id, bool buf2) {
	iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
	if (it != end()) {
		ChunkWeakRef &cwr = it->second;
		ChunkRef cr;
		if ((cr = cwr.lock())) {
			cr->setBlockId(rmod(x, CX), rmod(y, CY), rmod(z, CZ), id, buf2);
			return true;
		}
	}
	return false;
}

bool World::setBlockData(int x, int y, int z, BlockData data, bool buf2) {
	iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
	if (it != end()) {
		ChunkWeakRef &cwr = it->second;
		ChunkRef cr;
		if ((cr = cwr.lock())) {
			cr->setBlockData(rmod(x, CX), rmod(y, CY), rmod(z, CZ), data, buf2);
			return true;
		}
	}
	return false;
}

BlockId World::getBlockId(int x, int y, int z, bool buf2) {
	iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
	if (it != end()) {
		ChunkWeakRef &cwr = it->second;
		ChunkRef cr;
		if ((cr = cwr.lock())) {
			return cr->getBlockId(rmod(x, CX), rmod(y, CY), rmod(z, CZ), buf2);
		}
	}
	return Content::BlockIgnoreId;
}

BlockData World::getBlockData(int x, int y, int z, bool buf2) {
	iterator it = find(glm::ivec3(divrd(x, CX), divrd(y, CY), divrd(z, CZ)));
	if (it != end()) {
		ChunkWeakRef &cwr = it->second;
		ChunkRef cr;
		if ((cr = cwr.lock())) {
			return cr->getBlockData(rmod(x, CX), rmod(y, CY), rmod(z, CZ), buf2);
		}
	}
	return 0;
}

bool World::blockHasMetadata(int x, int y, int z, bool buf2) {
	return getBlockData(x, y, z, buf2) & BlockMetadataBit;
}

void World::render(const glm::mat4& transform) {
	if (Chunk::R.prog == nullptr)
		return;
	//lastVertCount = 0;
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
	ChunkRef c;
	for (auto pair : *this) {
		const glm::ivec3 &pos = pair.first;
		ChunkWeakRef &cwr = pair.second;
		if ((c = cwr.lock())) {
#if CHUNK_INMEM_COMPRESS
			if (!c->imcData && (G->TimeMs - c->imcUnusedSince) > CHUNK_INMEM_COMPRESS_DELAY)
				c->imcCompress();
#endif
			glm::vec3 translate = glm::vec3(pos.x * CX, pos.y * CY, pos.z * CZ);
			if (G->LP->camera.frustum.sphereInFrustum(translate + cShift, Chunk::CullSphereRadius)) {
				chunkTransform = glm::translate(transform, translate);
				c->render(chunkTransform);
				//lastVertCount += cc->vertices;
			}
		}
	}

	glDisableVertexAttribArray(Chunk::R.att_wave);
	glDisableVertexAttribArray(Chunk::R.att_color);
	glDisableVertexAttribArray(Chunk::R.att_texcoord);
	glDisableVertexAttribArray(Chunk::R.att_coord);
}

void World::renderTransparent(const glm::mat4 &transform) {
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
	ChunkRef c;
	for (auto pair : *this) {
		const glm::ivec3 &pos = pair.first;
		ChunkWeakRef &cwr = pair.second;
		if ((c = cwr.lock())) {
			glm::vec3 translate = glm::vec3(pos.x * CX, pos.y * CY, pos.z * CZ);
			if (G->LP->camera.frustum.sphereInFrustum(translate + cShift, Chunk::CullSphereRadius)) {
				chunkTransform = glm::translate(transform, translate);
				c->renderTransparent(chunkTransform);
				//lastVertCount += cc->vertices;
			}
		}
	}

	glDisableVertexAttribArray(Chunk::R.att_wave);
	glDisableVertexAttribArray(Chunk::R.att_color);
	glDisableVertexAttribArray(Chunk::R.att_texcoord);
	glDisableVertexAttribArray(Chunk::R.att_coord);
}

void World::onRenderPropertiesChanged() {
	ChunkRef c;
	if ((c = begin()->second.lock()))
		c->onRenderPropertiesChanged();
	refresh();
}

void World::refresh() {
	ChunkRef c;
	for (auto pair : *this)
		if ((c = pair.second.lock()))
			c->markAsDirty();
}

void World::write(OutStream &msg) const {
	const void *chunkData = nullptr;
	const uint dataSize = Chunk::AllocaSize;
	uint compressedSize;
	byte *compressed = new byte[dataSize];
	msg.writeU16(size());
	ChunkRef c;
	for (auto pair : *this) {
		if (!(c = pair.second.lock()))
			continue;
		chunkData = c->data;
		compressedSize = dataSize;
		int rz = lzfx_compress(chunkData, dataSize, compressed, &compressedSize);
		const glm::ivec3 &pos = pair.first;
		if (rz < 0) {
			getErrorStream() << "Failed compressing Chunk[" << pos.x << ',' << pos.y <<
				' ' << pos.z << ']' << std::endl;
		} else {
			msg.writeI16(pos.x);
			msg.writeI16(pos.y);
			msg.writeI16(pos.z);
			msg.writeU16(compressedSize);
			msg.writeData(compressed, compressedSize);
		}
	}
	delete[] compressed;
}

void World::read(InStream &M) {
	int bytesRead = 0;
	uint size = M.readU16();
	for (uint n=0; n < size; ++n) {
		int x = M.readI16(), y = M.readI16(), z = M.readI16();
		uint compressedSize = M.readU16();
		const uint targetDataSize = Chunk::AllocaSize;
		byte *compressedData = new byte[compressedSize];
		M.readData(compressedData, compressedSize);
		bytesRead += compressedSize;
		Chunk &c = *getChunkEx(x, y, z);
		uint outLen = targetDataSize;
		int rz = lzfx_decompress(compressedData, compressedSize, c.data, &outLen);
		if (rz < 0 || outLen != targetDataSize) {
			if (rz < 0) {
				getErrorStream() << "Chunk[" << x << ',' << y << ' ' << z <<
					"] LZFX decompression failed" << std::endl;
			} else {
				getErrorStream() << "Chunk[" << x << ',' << y << ' ' << z <<
					"] has bad size " << outLen << '/' << targetDataSize << std::endl;
			}
			// TODO: re-request?
		}
		delete[] compressedData;
	}
	getDebugStream() << "MapRead: read " << bytesRead << std::endl;
}

}