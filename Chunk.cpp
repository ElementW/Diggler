#include "Chunk.hpp"
#include "Platform.hpp"
#include "GlobalProperties.hpp"
#include "Blocks.hpp"
#include "Game.hpp"
#include "ChunkChangeHelper.hpp"
#include <cstring>
#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define CXY (CX*CY)
#define I(x,y,z) (x+y*CX+z*CXY)

namespace Diggler {

const Program *Chunk::RenderProgram = nullptr;
Texture *Chunk::TextureAtlas = nullptr;
Blocks *Chunk::BlkInf = nullptr;
GLint Chunk::RenderProgram_attrib_texcoord = -1;
GLint Chunk::RenderProgram_attrib_coord = -1;
GLint Chunk::RenderProgram_attrib_color = -1;
GLint Chunk::RenderProgram_uni_mvp = -1;

constexpr float Chunk::CullSphereRadius;
constexpr float Chunk::MidX, Chunk::MidY, Chunk::MidZ;

Chunk::Chunk(bool buffer, int scx, int scy, int scz, Game *G) : blk2(nullptr),
	scx(scx), scy(scy), scz(scz), G(G), vbo(nullptr), lavaCount(0) {
	changed = true;
	blk = new BlockType[CX*CY*CZ];
	for (int i=0; i < CX*CY*CZ; ++i)
		blk[i] = BlockType::Air;
	
	if (!buffer && GlobalProperties::IsClient) {
		vbo = new VBO;
		ibo = new VBO;
		if (RenderProgram == nullptr) {
			RenderProgram = G->PM->getProgram(PM_3D | PM_TEXTURED | PM_COLORED | PM_FOG);
			RenderProgram_attrib_coord = RenderProgram->att("coord");
			RenderProgram_attrib_color = RenderProgram->att("color");
			RenderProgram_attrib_texcoord = RenderProgram->att("texcoord");
			RenderProgram_uni_mvp = RenderProgram->uni("mvp");
			
			BlkInf = new Blocks();
			
			TextureAtlas = BlkInf->getAtlas();
		}
	}
	if (GlobalProperties::IsServer) {
		blk2 = new BlockType[CX*CY*CZ];
	}
}
 
Chunk::~Chunk() {
	delete blk;
	delete[] blk2;
	delete vbo; delete ibo;
}
 
BlockType Chunk::get(int x, int y, int z) {
	if ((x < 0 || y < 0 || z < 0 || x >= CX || y >= CY || z >= CZ) && G) {
		return G->SC->get(scx * CX + x, scy * CY + y, scz * CZ + z);
	}
	return blk[I(x,y,z)];
}

void Chunk::set2(int x, int y, int z, BlockType type) {
	if ((x < 0 || y < 0 || z < 0 || x >= CX || y >= CY || z >= CZ) && G)
		return G->SC->set2(scx * CX + x, scy * CY + y, scz * CZ + z, type);
	register BlockType *b = &(blk2[I(x,y,z)]);
	if (*b == BlockType::Lava)
		lavaCount--;
	if (type == BlockType::Lava)
		lavaCount++;
	*b = type;
	if (G && G->CCH)
		G->CCH->add(scx * CX + x, scy * CY + y, scz * CZ + z, type);
}

void Chunk::set(int x, int y, int z, BlockType type) {
	mut.lock();
	if ((x < 0 || y < 0 || z < 0 || x >= CX || y >= CY || z >= CZ) && G)
		return G->SC->set(scx * CX + x, scy * CY + y, scz * CZ + z, type);
	register BlockType *b = &(blk[I(x,y,z)]);
	if (*b == BlockType::Lava)
		lavaCount--;
	if (type == BlockType::Lava)
		lavaCount++;
	*b = type;
	changed = true;
	mut.unlock();
}

void Chunk::updateServerPrepare() {
	memcpy(blk2, blk, CX*CY*CZ*sizeof(BlockType));
}

void Chunk::updateServer() {
	if (lavaCount == 0)
		return;
	mut.lock();
	for (int x=0; x < CX; x++)
		for (int y=0; y < CY; y++)
			for (int z=0; z < CZ; z++) {
				if (blk[I(x,y,z)] == BlockType::Lava) {
					BlockType under = get(x, y-1, z);
					if (under == BlockType::Air) {
						set2(x, y-1, z, BlockType::Lava);
					} else if (under != BlockType::Lava) {
						if (get(x+1, y, z) == BlockType::Air)
							set2(x+1, y, z, BlockType::Lava);
						if (get(x-1, y, z) == BlockType::Air)
							set2(x-1, y, z, BlockType::Lava);
						if (get(x, y, z+1) == BlockType::Air)
							set2(x, y, z+1, BlockType::Lava);
						if (get(x, y, z-1) == BlockType::Air)
							set2(x, y, z-1, BlockType::Lava);
					}
				}
			}
	mut.unlock();
}

void Chunk::updateServerSwap() {
	std::swap(blk, blk2);
}

void Chunk::updateClient() {
	mut.lock();
	GLCoord		vertex[CX * CY * CZ * 6 /* faces */ * 6 /* vertices */ / 2 /* face removing (HSR) makes a lower vert max */];
	GLushort	index[CX * CY * CZ * 6 /* faces */ * 4 /* indexes */ / 2 /* HSR */];
	int v = 0, i = 0;

	BlockType bt;
	const AtlasCreator::Coord *tc;
	for(uint8 x = 0; x < CX; x++) {
		for(uint8 y = 0; y < CY; y++) {
			for(uint8 z = 0; z < CZ; z++) {
				bt = blk[I(x,y,z)];
				
				// Empty block?
				if (!bt)
					continue;

				// View from negative x
				if (Blocks::isFaceRemoved(bt, get(x - 1, y, z))) {
					index[i++] = v; index[i++] = v+1; index[i++] = v+2;
					index[i++] = v+3; index[i++] = v+2; index[i++] = v+1;
					tc = BlkInf->gTC(bt, FaceDirection::XDec);
					vertex[v++] = {x,     y,     z,     tc->x, tc->v, .6f, .6f, .6f};
					vertex[v++] = {x,     y,     z + 1, tc->u, tc->v, .6f, .6f, .6f};
					vertex[v++] = {x,     y + 1, z,     tc->x, tc->y, .6f, .6f, .6f};
					vertex[v++] = {x,     y + 1, z + 1, tc->u, tc->y, .6f, .6f, .6f};
				}

				// View from positive x
				if (Blocks::isFaceRemoved(bt, get(x + 1, y, z))) {
					index[i++] = v; index[i++] = v+1; index[i++] = v+2;
					index[i++] = v+3; index[i++] = v+2; index[i++] = v+1;
					tc = BlkInf->gTC(bt, FaceDirection::XInc);
					vertex[v++] = {x + 1, y,     z,     tc->u, tc->v, .6f, .6f, .6f};
					vertex[v++] = {x + 1, y + 1, z,     tc->u, tc->y, .6f, .6f, .6f};
					vertex[v++] = {x + 1, y,     z + 1, tc->x, tc->v, .6f, .6f, .6f};
					vertex[v++] = {x + 1, y + 1, z + 1, tc->x, tc->y, .6f, .6f, .6f};
				}

				// Negative Y
				if (Blocks::isFaceRemoved(bt, get(x, y - 1, z))) {
					index[i++] = v; index[i++] = v+1; index[i++] = v+2;
					index[i++] = v+3; index[i++] = v+2; index[i++] = v+1;
					float shade = (blk[I(x,y,z)] == BlockType::Shock) ? 1.5f : .2f;;
					tc = BlkInf->gTC(bt, FaceDirection::YDec);
					vertex[v++] = {x,     y,     z, tc->x, tc->v, shade, shade, shade};
					vertex[v++] = {x + 1, y,     z, tc->u, tc->v, shade, shade, shade};
					vertex[v++] = {x,     y, z + 1, tc->x, tc->y, shade, shade, shade};
					vertex[v++] = {x + 1, y, z + 1, tc->u, tc->y, shade, shade, shade};
				}

				// Positive Y
				if (Blocks::isFaceRemoved(bt, get(x, y + 1, z))) {
					index[i++] = v; index[i++] = v+1; index[i++] = v+2;
					index[i++] = v+3; index[i++] = v+2; index[i++] = v+1;
					tc = BlkInf->gTC(bt, FaceDirection::YInc);
					vertex[v++] = {x,     y + 1,     z, tc->u, tc->v, .8f, .8f, .8f};
					vertex[v++] = {x,     y + 1, z + 1, tc->u, tc->y, .8f, .8f, .8f};
					vertex[v++] = {x + 1, y + 1,     z, tc->x, tc->v, .8f, .8f, .8f};
					vertex[v++] = {x + 1, y + 1, z + 1, tc->x, tc->y, .8f, .8f, .8f};
				}

				// Negative Z
				if (Blocks::isFaceRemoved(bt, get(x, y, z - 1))) {
					index[i++] = v; index[i++] = v+1; index[i++] = v+2;
					index[i++] = v+3; index[i++] = v+2; index[i++] = v+1;
					tc = BlkInf->gTC(bt, FaceDirection::ZDec);
					vertex[v++] = {x,     y,     z, tc->u, tc->v, .4f, .4f, .4f};
					vertex[v++] = {x,     y + 1, z, tc->u, tc->y, .4f, .4f, .4f};
					vertex[v++] = {x + 1, y,     z, tc->x, tc->v, .4f, .4f, .4f};
					vertex[v++] = {x + 1, y + 1, z, tc->x, tc->y, .4f, .4f, .4f};
				}

				// Positive Z
				if (Blocks::isFaceRemoved(bt, get(x, y, z + 1))) {
					index[i++] = v; index[i++] = v+1; index[i++] = v+2;
					index[i++] = v+3; index[i++] = v+2; index[i++] = v+1;
					tc = BlkInf->gTC(bt, FaceDirection::ZInc);
					vertex[v++] = {x,     y,     z + 1, tc->x, tc->v, .4f, .4f, .4f};
					vertex[v++] = {x + 1, y,     z + 1, tc->u, tc->v, .4f, .4f, .4f};
					vertex[v++] = {x,     y + 1, z + 1, tc->x, tc->y, .4f, .4f, .4f};
					vertex[v++] = {x + 1, y + 1, z + 1, tc->u, tc->y, .4f, .4f, .4f};
				}
			}
		}
	}

	vertices = v;
	vbo->setData(vertex, v);
	indices = i;
	ibo->setData(index, i);
	changed = false;
	mut.unlock();
}

void Chunk::render(const glm::mat4 &transform) {
	if (changed)
		updateClient();
	if (!indices)
		return;
	
	RenderProgram->bind();
	
	glEnableVertexAttribArray(RenderProgram_attrib_coord);
	glEnableVertexAttribArray(RenderProgram_attrib_texcoord);
	glEnableVertexAttribArray(RenderProgram_attrib_color);
	glUniformMatrix4fv(RenderProgram_uni_mvp, 1, GL_FALSE, glm::value_ptr(transform));
	
	TextureAtlas->bind();
	vbo->bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->id);
	glVertexAttribPointer(RenderProgram_attrib_coord, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(GLCoord), 0);
	glVertexAttribPointer(RenderProgram_attrib_texcoord, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(GLCoord), (GLvoid*)offsetof(GLCoord, tx));
	glVertexAttribPointer(RenderProgram_attrib_color, 3, GL_FLOAT, GL_FALSE, sizeof(GLCoord), (GLvoid*)offsetof(GLCoord, r));
	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, nullptr);
	
	glDisableVertexAttribArray(RenderProgram_attrib_color);
	glDisableVertexAttribArray(RenderProgram_attrib_texcoord);
	glDisableVertexAttribArray(RenderProgram_attrib_coord);
}

void Chunk::renderBatched(const glm::mat4& transform) {
	if (changed)
		updateClient();
	if (!indices)
		return;
	
	glUniformMatrix4fv(RenderProgram_uni_mvp, 1, GL_FALSE, glm::value_ptr(transform));
	vbo->bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->id);
	glVertexAttribPointer(RenderProgram_attrib_coord, 3, GL_BYTE, GL_FALSE, sizeof(GLCoord), 0);
	glVertexAttribPointer(RenderProgram_attrib_texcoord, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(GLCoord), (GLvoid*)offsetof(GLCoord, tx));
	glVertexAttribPointer(RenderProgram_attrib_color, 3, GL_FLOAT, GL_FALSE, sizeof(GLCoord), (GLvoid*)offsetof(GLCoord, r));
	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, nullptr);
}

}