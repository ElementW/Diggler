#ifndef ESC_MENU_HPP
#define ESC_MENU_HPP

#include <memory>

#include <glm/glm.hpp>

#include "render/gl/Program.hpp"
#include "ui/Element.hpp"

namespace Diggler {

class Game;
namespace UI {
class Button;
class Text;
}

class EscMenu : public UI::Element {
private:
  Game *G;
  mutable struct {
    double start, duration;
    bool active;
  } m_transition;
  std::shared_ptr<UI::Text> txtMenuTitle;
  std::shared_ptr<UI::Button> m_button;
  struct MenuEntryImpl;
  struct MenuEntry {
    std::string text;
    std::unique_ptr<MenuEntryImpl> impl;
  };
  bool cursorIn; int cursorX, cursorY;
  std::vector<MenuEntry> entries;
  void refresh();

public:
  EscMenu(UI::Manager*);
  ~EscMenu();

  void addMenuEntry(const std::string &text);

  void onCursorMove(int x, int y) override;
  void onCursorLeave(int x, int y) override;
  void onInputAreaChanged() override;
  void setVisible(bool) override;

  void render(const glm::mat4&) const override;
  using UI::Element::render;
};

}

#endif
