#ifndef DIGGLER_RENDER_GL_PARTICLES_RENDERER_HPP
#define DIGGLER_RENDER_GL_PARTICLES_RENDERER_HPP

#include "../ParticlesRenderer.hpp"

#include "../../Program.hpp"
#include "VAO.hpp"
#include "VBO.hpp"

namespace Diggler {

class Game;

namespace Render {
namespace gl {

class GLParticlesRenderer : public ParticlesRenderer {
protected:
  Game *G;
  const Program *prog;
  GLuint  att_coord,
          att_color,
          att_texcoord,
          att_pointSize;
  GLint   uni_mvp,
          uni_unicolor,
          uni_fogStart,
          uni_fogEnd;
  struct EmitterRenderData {
    VAO vao;
    VBO vbo;
  };

public:
  GLParticlesRenderer(Game*);
  ~GLParticlesRenderer();

  void registerEmitter(ParticleEmitter*);
  void updateParticleData(ParticleEmitter*, ParticleEmitter::ParticleRenderData *data, uint count);
  void unregisterEmitter(ParticleEmitter*);

  void render(RenderParams&);
};

}
}
}

#endif /* DIGGLER_RENDER_GL_PARTICLES_RENDERER_HPP */
