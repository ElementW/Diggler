#ifndef DIGGLER_RENDER_GL_WORLD_RENDERER_HPP
#define DIGGLER_RENDER_GL_WORLD_RENDERER_HPP

#include "../WorldRenderer.hpp"

#include <vector>

#include "../../Program.hpp"
#include "VAO.hpp"
#include "VBO.hpp"

namespace Diggler {

class Game;

namespace Render {
namespace gl {

class GLWorldRenderer : public WorldRenderer {
protected:
  Game *G;
  const Program *prog;
  GLuint  att_coord,
          att_color,
          att_texcoord,
          att_wave;
  GLint   uni_mvp,
          uni_unicolor,
          uni_fogStart,
          uni_fogEnd,
          uni_time;
  const Texture *TextureAtlas;

  struct ChunkEntry {
    Render::gl::VBO vbo, ibo;
    uint vertCount, indicesOpq, indicesTpt;
  };
  std::vector<ChunkEntry> m_chunks;

  void loadShader();

public:
  GLWorldRenderer(Game*);
  ~GLWorldRenderer();

  void registerChunk(Chunk*);
  void updateChunk(Chunk*, Chunk::Vertex *vertices, uint vertCount, uint16 *indicesOpq,
    uint idxOpqCount, uint16 *indicesTpt, uint idxTptCount);
  void unregisterChunk(Chunk*);

  void render(RenderParams&);
};

}
}
}

#endif /* DIGGLER_RENDER_GL_WORLD_RENDERER_HPP */
