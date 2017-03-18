#include "EscMenu.hpp"

#include "render/gl/OpenGL.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Game.hpp"
#include "GameWindow.hpp"
#include "ui/Button.hpp"
#include "ui/Manager.hpp"
#include "ui/Text.hpp"

namespace Diggler {

struct EscMenu::MenuEntryImpl {
  Area inputArea;
  std::shared_ptr<UI::Text> txtText;
};

EscMenu::EscMenu(UI::Manager *UIM) :
  UI::Element(UIM),
  G(UIM->G) {
  txtMenuTitle = G->UIM->addManual<UI::Text>(" Menu", 3, 3);
  //m_button = new UIButton(G, glm::mat);
  for (int i=0;i<10;++i)
    addMenuEntry("hello " + std::to_string(i));
}

EscMenu::~EscMenu() {

}

void EscMenu::refresh() {
  int y = inputArea().h;
  y -= txtMenuTitle->getSize().y;

  for (const MenuEntry &me : entries) {
    const int dec = (me.impl->txtText->getSize().y * 3) / 2;
    y -= dec;
    me.impl->inputArea = Area(0, y, inputArea().w, dec);
  }
}

void EscMenu::addMenuEntry(const std::string &text) {
  entries.emplace_back(MenuEntry { text, std::make_unique<MenuEntryImpl>() });
  entries.back().impl->txtText = G->UIM->addManual<UI::Text>(text, 2, 2);
  refresh();
}

void EscMenu::onCursorMove(int x, int y) {
  cursorIn = true;
  cursorX = x;
  cursorY = y;
}

void EscMenu::onCursorLeave(int, int) {
  cursorIn = false;
}

void EscMenu::onInputAreaChanged() {
  refresh();
}

void EscMenu::setVisible(bool v) {
  UI::Element::setVisible(v);
  if (v) {
    m_transition.start = G->Time;
    m_transition.duration = .3;
    m_transition.active = true;
  }
}

static double easeOutQuart(double t, double d) {
  t /= d;
  t--;
  return -(t*t*t*t - 1);
}

void EscMenu::render(const glm::mat4 &baseMatrix) const {
  double scroll;
  if (m_transition.active) {
    scroll = easeOutQuart(G->Time-m_transition.start, m_transition.duration);
    if (G->Time-m_transition.start >= m_transition.duration) {
      m_transition.active = false;
    }
  } else {
    scroll = 1;
  }

  const int width = renderArea().w;
  const int pxScroll = (1 - scroll) * width;
  G->UIM->drawRect(baseMatrix, UI::Element::Area { pxScroll, 0, width, renderArea().h },
      glm::vec4(0.f, 0.f, 0.f, 0.8f));

  int y = renderArea().h;
  y -= txtMenuTitle->getSize().y;
  glm::mat4 matrix = glm::translate(baseMatrix, glm::vec3(pxScroll, y, 0));
  txtMenuTitle->render(matrix);

  for (const MenuEntry &me : entries) {
    if (cursorIn and me.impl->inputArea.isIn(cursorX, cursorY)) {
      G->UIM->drawRect(baseMatrix, me.impl->inputArea,
          glm::vec4(1.f, 0.f, 0.f, 0.8f));
    }
    const int height = me.impl->txtText->getSize().y;
    y -= (height * 3) / 2;
    matrix = glm::translate(baseMatrix, glm::vec3(pxScroll, y, 0));
    me.impl->txtText->render(matrix);
  }
}

}
