#ifndef DIGGLER_RENDER_GL_RENDERER_HPP
#define DIGGLER_RENDER_GL_RENDERER_HPP

#include "../Renderer.hpp"

#include "FeatureSupport.hpp"
#include "ParticlesRenderer.hpp"
#include "WorldRenderer.hpp"

namespace Diggler {
namespace Render {
namespace gl {

class GLRenderer : public Renderer {
public:
  GLRenderer(Game *G);

  void beginFrame() override;
  void endFrame() override;
};

}
}
}

#endif /* DIGGLER_RENDER_GL_RENDERER_HPP */
