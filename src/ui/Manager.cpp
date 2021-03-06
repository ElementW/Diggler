#include "Manager.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../render/gl/VAO.hpp"
#include "../render/gl/VBO.hpp"
#include "../Texture.hpp"
#include "../render/gl/Program.hpp"
#include "../render/gl/ProgramManager.hpp"
#include "../Game.hpp"
#include "../GameWindow.hpp"

namespace diggler {
namespace ui {

struct Renderer {
  const render::gl::Program *prog;
  GLint att_texcoord, att_coord, uni_mvp, uni_unicolor;
} R{}, RR{};

Manager::Manager() :
  scale(2) {
  PM = &m_projMatrix;
  m_projMat1 = glm::ortho(0.f, 1.f, 0.f, 1.f);
  m_projMat1V = glm::ortho(0.f, 1.f, 1.f, 0.f);
  PM1 = &m_projMat1;
}

void Manager::setup(Game *G) {
  this->G = G;
  if (R.prog == nullptr) {
    R.prog = G->PM->getProgram("2d", "texture0", "texcoord0");
    R.att_texcoord = R.prog->att("texcoord");
    R.att_coord = R.prog->att("coord");
    R.uni_mvp = R.prog->uni("mvp");
    R.uni_unicolor = R.prog->uni("unicolor");
  }
  if (RR.prog == nullptr) {
    RR.prog = G->PM->getProgram("2d");
    RR.att_coord = RR.prog->att("coord");
    RR.uni_mvp = RR.prog->uni("mvp");
    RR.uni_unicolor = RR.prog->uni("unicolor");
  }
  m_rectVbo.reset(new render::gl::VBO);
  const uint8 verts[6*4] = {
    0, 0, 0, 1,
    1, 0, 1, 1,
    0, 1, 0, 0,
    
    0, 1, 0, 0,
    1, 0, 1, 1,
    1, 1, 1, 0
  };
  m_rectVbo->setData(verts, 6*4);
  m_rectVao = std::make_unique<render::gl::VAO>();
  { render::gl::VAO::Config cfg = m_rectVao->configure();
    cfg.vertexAttrib(*m_rectVbo, RR.att_coord, 2, GL_UNSIGNED_BYTE, 4*sizeof(uint8));
    cfg.vertexAttrib(*m_rectVbo, R.att_texcoord, 2, GL_UNSIGNED_BYTE, 4*sizeof(uint8), 2*sizeof(uint8));
    cfg.commit();
  }
}


void Manager::onCursorPos(double x, double y) {
  y = G->GW->getH() - y;

  std::shared_ptr<Element> hovered;
  for (std::weak_ptr<Element> &elmPtr : m_elements) {
    std::shared_ptr<Element> elm = elmPtr.lock();
    if (!elm) {
      continue;
    }
    const Element::Area &a = elm->inputArea();
    if (!elm->m_manualInput &&
        elm->m_isVisible &&
        a.isIn(x, y) &&
        !elm->cursorPassesThrough(x - a.x, y - a.y)) {
      hovered = std::move(elm);
      break;
    }
  }
  auto lastHoveredElement = m_hoveredElement.lock();
  if (hovered != lastHoveredElement) {
    if (lastHoveredElement) {
      lastHoveredElement->m_isCursorOver = false;
      const Element::Area &a = lastHoveredElement->inputArea();
      lastHoveredElement->onCursorLeave(x - a.x, y - a.y);
    }
    if (hovered) {
      hovered->m_isCursorOver = true;
      const Element::Area &a = hovered->inputArea();
      hovered->onCursorEnter(x - a.x, y - a.y);
    }
    m_hoveredElement = hovered;
  }
  if (hovered) {
    const Element::Area &a = hovered->inputArea();
    hovered->onCursorMove(x - a.x, y - a.y);
  }
}

void Manager::onMouseButton(int key, int action, int mods) {
  auto hoveredElement = m_hoveredElement.lock();
  if (hoveredElement) {
    double x, y;
    glfwGetCursorPos(*G->GW, &x, &y);
    Element::MouseButton btn = Element::MouseButton::Unknown;
    switch (key) {
    case GLFW_MOUSE_BUTTON_LEFT:
      btn = Element::MouseButton::Left;
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      btn = Element::MouseButton::Middle;
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      btn = Element::MouseButton::Right;
      break;
    }
    const Element::Area &a = hoveredElement->inputArea();
    if (action == GLFW_PRESS) {
      hoveredElement->onMouseDown(x - a.x, y - a.y, btn);
      setFocused(m_hoveredElement);
    } else if (action == GLFW_RELEASE) {
      hoveredElement->onMouseUp(x - a.x, y - a.y, btn);
      setFocused(m_hoveredElement);
    }
  }
}

void Manager::onMouseScroll(double x, double y) {
  auto hoveredElement = m_hoveredElement.lock();
  if (hoveredElement) {
    setFocused(hoveredElement);
    hoveredElement->onMouseScroll(x, y);
  }
}

void Manager::onKey(int key, int scancode, int action, int mods) {
  auto focusedElement = m_focusedElement.lock();
  if (focusedElement) {
    //TODO
  }
}

void Manager::onChar(char32 unichar) {
  auto focusedElement = m_focusedElement.lock();
  if (focusedElement) {
    //TODO
  }
}

void Manager::onResize(int w, int h) {
  setProjMat(glm::ortho(0.0f, (float)w, 0.0f, (float)h));
}

void Manager::add(std::weak_ptr<Element> elm) {
  m_elements.emplace_back(elm);
}

std::weak_ptr<Element> Manager::getFocused() const {
  return m_focusedElement;
}

void Manager::setFocused(const std::weak_ptr<Element> &elmPtr) {
  auto elm = elmPtr.lock();
  auto hoveredElement = m_hoveredElement.lock();
  if (elm != hoveredElement) {
    if (hoveredElement) {
      hoveredElement->m_hasFocus = false;
      hoveredElement->onFocusLost();
    }
    if (elm) {
      elm->m_hasFocus = true;
      elm->onFocus();
    }
    m_focusedElement = elm;
  }
}

void Manager::setFocused(decltype(nullptr)) {
  setFocused(std::shared_ptr<Element>());
}

void Manager::render() {
  for (auto it = m_elements.cbegin(); it != m_elements.cend();) {
    auto elm = it->lock();
    if (!elm) {
      it = m_elements.erase(it);
      continue;
    }
    if (!elm->m_manualRender && elm->m_isVisible) {
      elm->render();
    }
    ++it;
  }
}

void Manager::setProjMat(const glm::mat4 &mat) {
  m_projMatrix = mat;
}

void Manager::drawRect(const glm::mat4 &mat, const glm::vec4 &color) const {
  RR.prog->bind();
  m_rectVao->bind();
  Texture::unbind();
  glUniform4f(RR.uni_unicolor, color.r, color.g, color.b, color.a);
  glUniformMatrix4fv(RR.uni_mvp, 1, GL_FALSE, glm::value_ptr(mat));
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // OpenGL needs to be stateless. Definitely. Wait for Vulkan.
  glUniform4f(RR.uni_unicolor, 1.f, 1.f, 1.f, 1.f);

  m_rectVao->unbind();
}

void Manager::drawRect(const glm::mat4 &m, const Element::Area &a, const glm::vec4 &color) const {
  return drawRect(glm::scale(glm::translate(m, glm::vec3(a.x, a.y, 0)), glm::vec3(a.w, a.h, 0)), color);
}

void Manager::drawRect(const glm::mat4 &m, int w, int h, const glm::vec4 &color) const {
  return drawRect(glm::scale(m, glm::vec3(w, h, 0)), color);
}

void Manager::drawFullRect(const glm::vec4 &color) const {
  drawRect(m_projMat1, color);
}

void Manager::drawTex(const glm::mat4 &mat, const Texture &t, const glm::vec4 &color) const {
  R.prog->bind();

  t.bind();
  m_rectVao->bind();
  glUniform4f(R.uni_unicolor, color.r, color.g, color.b, color.a);
  glUniformMatrix4fv(R.uni_mvp, 1, GL_FALSE, glm::value_ptr(mat));
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glUniform4f(R.uni_unicolor, 1.f, 1.f, 1.f, 1.f);

  m_rectVao->unbind();
}

void Manager::drawTex(const glm::mat4 &mat, const Texture &t) const {
  drawTex(mat, t, glm::vec4(1.f));
}

void Manager::drawTex(const glm::mat4 &m, const Element::Area &a, const Texture &t) const {
  drawTex(glm::scale(glm::translate(m, glm::vec3(a.x, a.y, 0)), glm::vec3(a.w, a.h, 0)), t);
}

void Manager::drawTex(const glm::mat4 &m, int w, int h, const Texture &t) const {
  drawTex(glm::scale(m, glm::vec3(w, h, 0)), t);
}

void Manager::drawFullTexV(const Texture &t) const {
  drawTex(m_projMat1V, t);
}

void Manager::drawFullTexV(const Texture &t, const glm::vec4 &color) const {
  drawTex(m_projMat1V, t, color);
}

}
}
