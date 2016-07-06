#ifndef DIGGLER_RENDER_GL_RENDERER_HPP
#define DIGGLER_RENDER_GL_RENDERER_HPP

#include "../Renderer.hpp"

#include "FeatureSupport.hpp"
#include "ParticlesRenderer.hpp"

namespace Diggler {
namespace Render {
namespace gl {

class GLRenderer : public Renderer {
public:
  GLRenderer(Game *G) :
    Renderer(G) {
    FeatureSupport::probe();
    PR = new GLParticlesRenderer(G);
  }
};

}
}
}

#endif /* DIGGLER_RENDER_GL_RENDERER_HPP */
