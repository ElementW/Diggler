#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <glm/detail/type_mat.hpp>

#include "render/gl/VBO.hpp"
#include "Game.hpp"

namespace Diggler {

namespace Render {
namespace gl {
class Program;
}
}

class Texture;

class Skybox {
private:
  static const Render::gl::Program *RenderProgram;
  static GLint RenderProgram_attrib_coord, RenderProgram_attrib_texcoord, RenderProgram_uni_mvp;
  Render::gl::VBO m_vbo;
  Texture *m_top, *m_w, *m_e, *m_n, *m_s, *m_bottom;
  Game *G;
  struct Coord { int8 x, y, z, u, v; };

public:
  Skybox(Game *G, const std::string &skyName);
  ~Skybox();
  void render(const glm::mat4 &transform) const;
};

}

#endif
