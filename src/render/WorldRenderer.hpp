#ifndef DIGGLER_RENDER_WORLD_RENDERER_HPP
#define DIGGLER_RENDER_WORLD_RENDERER_HPP

#include "RenderParams.hpp"
#include "../World.hpp"

namespace diggler {
namespace render {

class WorldRenderer {
protected:
  inline uintptr_t getRendererData(const Chunk *c) const {
    return c->rendererData;
  }
  inline void setRendererData(Chunk *c, uintptr_t data) const {
    c->rendererData = data;
  }

public:
  struct PointedHighlight {
    virtual ~PointedHighlight() {}
    virtual void setVisible(bool) = 0;
    virtual void setColor(uint8 r, uint8 g, uint8 b, uint8 a) = 0;
    virtual void setCenter(const glm::vec3&) = 0;
  } &pointedHighlight;

  WorldRenderer(PointedHighlight &ph) :
    pointedHighlight(ph) {
  }
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
