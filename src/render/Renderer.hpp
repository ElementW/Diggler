#ifndef DIGGLER_RENDER_RENDERER_HPP
#define DIGGLER_RENDER_RENDERER_HPP

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

  struct Renderers {
    ParticlesRenderer *particles;
    WorldRenderer *world;
  } renderers;
};

inline Renderer::~Renderer() {}

}
}

#endif /* DIGGLER_RENDER_RENDERER_HPP */
