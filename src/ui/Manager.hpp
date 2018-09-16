#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include <list>
#include <memory>

#include "../platform/types/mat4.hpp"
#include "../platform/types/vec4.hpp"

#include "Element.hpp"

namespace diggler {

namespace render {
namespace gl {
class VAO;
class VBO;
}
}
class GameWindow;
class Texture;

namespace ui {

class Manager {
private:
  std::unique_ptr<render::gl::VBO> m_rectVbo;
  std::unique_ptr<render::gl::VAO> m_rectVao;
  std::list<std::weak_ptr<Element>> m_elements;
  std::weak_ptr<Element> m_hoveredElement;
  std::weak_ptr<Element> m_focusedElement;
  mat4 m_projMatrix, m_projMat1, m_projMat1V;
  
  friend GameWindow;
  void setProjMat(const mat4&);
  void add(std::weak_ptr<Element>);
  template<class T, typename... Args> std::shared_ptr<T> create(Args&&... args) {
    return std::make_shared<T>(this, std::forward<Args>(args)...);
  }

public:
  const mat4 *PM, *PM1;
  Game *G;
  int scale;

  Manager();
  void setup(Game*);

  void onCursorPos(double x, double y);
  void onMouseButton(int key, int action, int mods);
  void onMouseScroll(double x, double y);
  void onKey(int key, int scancode, int action, int mods);
  void onChar(char32 unichar);
  void onResize(int w, int h);

  template<class T, typename... Args> std::shared_ptr<T> add(Args&&... args) {
    std::shared_ptr<T> obj = create<T>(std::forward<Args>(args)...);
    add(obj);
    return obj;
  }

  template<class T, typename... Args> std::shared_ptr<T> addManual(Args&&... args) {
    std::shared_ptr<T> obj = create<T>(std::forward<Args>(args)...);
    obj->setManual();
    add(obj);
    return obj;
  }

  std::weak_ptr<Element> getFocused() const;
  void setFocused(const std::weak_ptr<Element>&);
  void setFocused(decltype(nullptr));

  // Utility

  void drawRect(const mat4&, const Element::Area&, const vec4 &color) const;
  void drawRect(const mat4&, int w, int h, const vec4 &color) const;
  void drawRect(const mat4&, const vec4 &color) const;
  void drawFullRect(const vec4 &color) const;
  void drawTex(const mat4&, const Element::Area&, const Texture&) const;
  void drawTex(const mat4&, int w, int h, const Texture&) const;
  void drawTex(const mat4&, const Texture&) const;
  void drawTex(const mat4&, const Texture&, const vec4 &color) const;
  void drawFullTexV(const Texture&) const;
  void drawFullTexV(const Texture&, const vec4 &color) const;

  // Render

  void render();
};

}
}

#endif
