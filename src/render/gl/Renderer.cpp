#include "Renderer.hpp"

#include "../../util/Log.hpp"
#include "FeatureSupport.hpp"
#include "FontRenderer.hpp"
#include "ParticlesRenderer.hpp"
#include "WorldRenderer.hpp"

namespace Diggler {
namespace Render {
namespace gl {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "GLRenderer";

GLRenderer::GLRenderer(Game *G) :
  Renderer(G) {
  FeatureSupport::probe();
  Log(Verbose, TAG) << "GLRenderer, using features: " << FeatureSupport::supported();
  renderers.font = new GLFontRenderer(G);
  renderers.particles = new GLParticlesRenderer(G);
  renderers.world = new GLWorldRenderer(G);
}

GLRenderer::~GLRenderer() {
  delete renderers.font;
  delete renderers.particles;
  delete renderers.world;
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
