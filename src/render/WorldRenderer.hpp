#ifndef DIGGLER_RENDER_WORLD_RENDERER_HPP
#define DIGGLER_RENDER_WORLD_RENDERER_HPP

#include "RenderParams.hpp"
#include "../World.hpp"

namespace Diggler {
namespace Render {

class WorldRenderer {
protected:
  inline uintptr_t getRendererData(const Chunk *c) const {
    return c->rendererData;
  }
  inline void setRendererData(Chunk *c, uintptr_t data) const {
    c->rendererData = data;
  }

public:
  virtual ~WorldRenderer() = 0;

  virtual void registerChunk(Chunk*) = 0;
  virtual void updateChunk(Chunk*, Chunk::Vertex *vertices, uint vertCount, uint16 *indicesOpq,
    uint idxOpqCount, uint16 *indicesTpt, uint idxTptCount) = 0;
  virtual void unregisterChunk(Chunk*) = 0;

  virtual void render(RenderParams&) = 0;
};

inline WorldRenderer::~WorldRenderer() {}

}
}

#endif /* DIGGLER_RENDER_WORLD_RENDERER_HPP */
