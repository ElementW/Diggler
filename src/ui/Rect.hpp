#ifndef RECT_HPP
#define RECT_HPP
#include "Element.hpp"

namespace Diggler {
namespace UI {

class Rect : public Element {
private:
  class VBO m_vbo;
  Color m_color;
  glm::mat4 m_mat;
  class Texture *m_tex;

public:
  Rect();
  Rect(int x = 0, int y = 0, int w = 1, int h = 1);
  void create(int x = 0, int y = 0, int w = 1, int h = 1);
  
  void setPos(int x, int y);
  void setSize(int w, int h);
  void setColor(float r, float g, float b, float a = 1.f);
  /// Sets the texture the Rect must be drawn with. Can be nullptr
  void setTexture(Texture*);
  
  void onMatrixChange();
  
  void render() const;
};

}
}

#endif