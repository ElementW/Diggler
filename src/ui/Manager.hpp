#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP
#include <list>
#include <memory>
#include "Element.hpp"

namespace Diggler {

class VBO;
class GameWindow;
class Texture;

namespace UI {

class Manager {
private:
  std::unique_ptr<VBO> m_rectVbo;
  std::list<std::unique_ptr<Element>> m_elements;
  Element *m_hoveredElement;
  Element *m_focusedElement;
  glm::mat4 m_projMatrix, m_projMat1, m_projMat1V;
  
  friend GameWindow;
  void setProjMat(const glm::mat4&);

public:
  const glm::mat4 *PM, *PM1;
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
  
  template<class T, typename... Args> T* add(Args&&... args) {
    T *obj = new T(this, std::forward<Args>(args)...);
    m_elements.emplace_back(reinterpret_cast<Element*>(obj));
    return obj;
  }
  void add(Element*);
  template<class T, typename... Args> T* create(Args&&... args) {
    return new T(this, std::forward<Args>(args)...);
  }
  void remove(Element*);
  void clear();

  Element* getFocused() const;
  void setFocused(Element*);

  // Utility

  void drawRect(const Element::Area&, const glm::vec4 &color) const;
  void drawRect(const glm::mat4&, const glm::vec4 &color) const;
  void drawFullRect(const glm::vec4 &color) const;
  void drawTex(const Element::Area&, const Texture&) const;
  void drawTex(const glm::mat4&, const Texture&) const;
  void drawTex(const glm::mat4&, const Texture&, const glm::vec4 &color) const;
  void drawFullTexV(const Texture&) const;
  void drawFullTexV(const Texture&, const glm::vec4 &color) const;

  // Render

  void render();
};

}
}

#endif