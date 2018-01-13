#ifndef ESC_MENU_HPP
#define ESC_MENU_HPP

#include <memory>

#include <glm/glm.hpp>

#include "render/gl/Program.hpp"
#include "ui/Element.hpp"

namespace diggler {

class Game;
namespace ui {
class Button;
class Text;
}

class EscMenu : public ui::Element {
private:
  Game *G;
  mutable struct {
    double start, duration;
    bool active;
  } m_transition;
  std::shared_ptr<ui::Text> txtMenuTitle;
  std::shared_ptr<ui::Button> m_button;
  struct MenuEntryImpl;
  struct MenuEntry {
    std::string text;
    std::unique_ptr<MenuEntryImpl> impl;
  };
  bool cursorIn; int cursorX, cursorY;
  std::vector<MenuEntry> entries;
  void refresh();

public:
  EscMenu(ui::Manager*);
  ~EscMenu();

  void addMenuEntry(const std::string &text);

  void onCursorMove(int x, int y) override;
  void onCursorLeave(int x, int y) override;
  void onInputAreaChanged() override;
  void setVisible(bool) override;

  void render(const glm::mat4&) const override;
  using ui::Element::render;
};

}

#endif
