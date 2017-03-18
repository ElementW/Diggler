#include "Renderer.hpp"

#include "../../util/Log.hpp"
#include "DelegateGL.hpp"
#include "FeatureSupport.hpp"
#include "FontRenderer.hpp"
#include "ParticlesRenderer.hpp"
#include "TextureManager.hpp"
#include "WorldRenderer.hpp"

namespace Diggler {
namespace Render {
namespace gl {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "GLRenderer";

GLRenderer::GLRenderer(Game *G) :
  Renderer(G) {
  DelegateGL::GLThreadId = std::this_thread::get_id();
  FeatureSupport::probe();
  Log(Verbose, TAG) << "GLRenderer, using features: " << FeatureSupport::supported();
  renderers.font = std::make_unique<GLFontRenderer>(G);
  renderers.particles = std::make_unique<GLParticlesRenderer>(G);
  renderers.world = std::make_unique<GLWorldRenderer>(G);
  textureManager = std::make_unique<TextureManager>(*G);
}

GLRenderer::~GLRenderer() {
}

void GLRenderer::beginFrame() {
  DelegateGL::execute();
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::endFrame() {
  DelegateGL::execute();
}

}
}
}
