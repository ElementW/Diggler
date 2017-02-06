#ifndef DIGGLER_RENDER_RENDERER_HPP
#define DIGGLER_RENDER_RENDERER_HPP

#include "FontRenderer.hpp"
#include "ParticlesRenderer.hpp"
#include "WorldRenderer.hpp"

namespace Diggler {

class Game;

namespace Render {

class Renderer {
protected:
  Game *G;

  Renderer(Game *G) :
    G(G) {
  }

public:
  virtual ~Renderer() = 0;

  virtual void beginFrame() = 0;
  virtual void endFrame() = 0;

  struct Renderers {
    FontRenderer *font;
    ParticlesRenderer *particles;
    WorldRenderer *world;
  } renderers;
};

inline Renderer::~Renderer() {}

}
}

#endif /* DIGGLER_RENDER_RENDERER_HPP */
