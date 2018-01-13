#ifndef DIGGLER_RENDER_GL_RENDERER_HPP
#define DIGGLER_RENDER_GL_RENDERER_HPP

#include "../Renderer.hpp"

namespace diggler {
namespace render {
namespace gl {

class GLRenderer : public Renderer {
public:
  GLRenderer(Game *G);
  ~GLRenderer();

  void beginFrame() override;
  void endFrame() override;
};

}
}
}

#endif /* DIGGLER_RENDER_GL_RENDERER_HPP */
