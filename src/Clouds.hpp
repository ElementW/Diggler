#ifndef CLOUDS_HPP
#define CLOUDS_HPP

#include <epoxy/gl.h>
#include <glm/glm.hpp>

#include "render/gl/VBO.hpp"

namespace Diggler {

class Game;
class Texture;

namespace Render {
namespace gl {
class Program;
}
}

class Clouds {
private:
  static struct Renderer {
    const Render::gl::Program *prog;
    GLint att_coord, att_texcoord, uni_mvp, uni_texshift;
  } R;
  Texture **m_tex;
  Render::gl::VBO m_vbo;
  int m_layers;
  Game *G;
  struct Coord { uint8 x, y, z, u, v; };

public:
  Clouds(Game *G, int w, int h, int layers);
  void render(const glm::mat4 &transform);
  ~Clouds();
};

}

#endif
