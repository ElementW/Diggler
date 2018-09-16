#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "platform/types/mat4.hpp"

#include "render/gl/VBO.hpp"
#include "Game.hpp"

namespace diggler {

namespace render {
namespace gl {
class Program;
}
}

class Texture;

class Skybox {
private:
  static const render::gl::Program *RenderProgram;
  static GLint RenderProgram_attrib_coord, RenderProgram_attrib_texcoord, RenderProgram_uni_mvp;
  render::gl::VBO m_vbo;
  Texture *m_top, *m_w, *m_e, *m_n, *m_s, *m_bottom;
  Game *G;
  struct Coord { int8 x, y, z, u, v; };

public:
  Skybox(Game *G, const std::string &skyName);
  ~Skybox();
  void render(const mat4 &transform) const;
};

}

#endif
