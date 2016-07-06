#ifndef DIGGLER_RENDER_GL_RENDERER_HPP
#define DIGGLER_RENDER_GL_RENDERER_HPP

#include "../Renderer.hpp"

#include "ParticlesRenderer.hpp"

namespace Diggler {
namespace Render {
namespace gl {

class GLRenderer : public Renderer {
public:
  GLRenderer(Game *G) :
    Renderer(G) {
    PR = new GLParticlesRenderer(G);
  }
};

}
}
}

#endif /* DIGGLER_RENDER_GL_RENDERER_HPP */
