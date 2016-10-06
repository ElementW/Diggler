#include "Renderer.hpp"

namespace Diggler {
namespace Render {
namespace gl {

GLRenderer::GLRenderer(Game *G) :
  Renderer(G) {
  FeatureSupport::probe();
  getDebugStream() << "GLRenderer, using features: " << FeatureSupport::supported() << std::endl;
  renderers.particles = new GLParticlesRenderer(G);
  renderers.world = new GLWorldRenderer(G);
}

void GLRenderer::beginFrame() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::endFrame() {

}

}
}
}
