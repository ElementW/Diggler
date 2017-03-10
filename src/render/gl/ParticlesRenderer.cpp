#include "ParticlesRenderer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../Game.hpp"
#include "ProgramManager.hpp"

namespace Diggler {
namespace Render {
namespace gl {

using GLParticle = ParticleEmitter::ParticleRenderData;

GLParticlesRenderer::GLParticlesRenderer(Game *G) :
  G(G) {
  prog = G->PM->getProgram("3d", "pointSize", "color0", "fog0");
  att_coord = prog->att("coord");
  att_color = prog->att("color");
  //att_texcoord = prog->att("texcoord");
  att_pointSize = prog->att("pointSize");
  uni_mvp = prog->uni("mvp");
  uni_unicolor = prog->uni("unicolor");
  uni_fogStart = prog->uni("fogStart");
  uni_fogEnd = prog->uni("fogEnd");
}

GLParticlesRenderer::~GLParticlesRenderer() {
}

void GLParticlesRenderer::registerEmitter(ParticleEmitter &pe) {
  EmitterRenderData &rd = *(new EmitterRenderData);
  setRendererData(pe, reinterpret_cast<uintptr_t>(&rd));
  { VAO::Config cfg = rd.vao.configure();
    cfg.vertexAttrib(rd.vbo, att_coord, 3, GL_FLOAT, sizeof(GLParticle), 0);
    cfg.vertexAttrib(rd.vbo, att_color, 4, GL_FLOAT, sizeof(GLParticle),
      offsetof(GLParticle, r));
    cfg.vertexAttrib(rd.vbo, att_pointSize, 1, GL_FLOAT, sizeof(GLParticle),
      offsetof(GLParticle, s));
    cfg.commit();
  }
}

void GLParticlesRenderer::updateParticleData(ParticleEmitter &pe,
  ParticleEmitter::ParticleRenderData *data, size_t count) {
  EmitterRenderData &rd = *reinterpret_cast<EmitterRenderData*>(getRendererData(pe));
  rd.vbo.setDataGrow(data, count, GL_STREAM_DRAW);
}

void GLParticlesRenderer::unregisterEmitter(ParticleEmitter &pe) {
  delete reinterpret_cast<EmitterRenderData*>(getRendererData(pe));
}

void GLParticlesRenderer::render(RenderParams &rp) {
  prog->bind();
  glEnableVertexAttribArray(att_coord);
  glEnableVertexAttribArray(att_color);
  glEnableVertexAttribArray(att_pointSize);
  glUniformMatrix4fv(uni_mvp, 1, GL_FALSE, glm::value_ptr(rp.transform));
  glUniform1f(uni_fogStart, G->RP->fogStart);
  glUniform1f(uni_fogEnd, G->RP->fogEnd);
  glUniform4f(uni_unicolor, 1.f, 1.f, 1.f, 1.f);

  for (ParticleEmitter &pe : rp.world->emitters) {
    EmitterRenderData &rd = *reinterpret_cast<EmitterRenderData*>(getRendererData(pe));
    rd.vao.bind();
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(pe.getMaxCount()));
  }

  glDisableVertexAttribArray(att_pointSize);
  glDisableVertexAttribArray(att_color);
  glDisableVertexAttribArray(att_coord);
}

}
}
}
