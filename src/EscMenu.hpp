#ifndef ESC_MENU_HPP
#define ESC_MENU_HPP

#include <memory>

#include <glm/glm.hpp>

#include "render/gl/Program.hpp"

namespace Diggler {

class Game;
namespace UI {
class Button;
class Text;
}

class EscMenu {
private:
  Game *G;
  struct {
    double start, duration;
    bool active;
  } m_transition;
  std::shared_ptr<UI::Text> txt_quit;
  glm::mat4 matrix;
  std::shared_ptr<UI::Button> m_button;

public:
  EscMenu(Game *G);
  ~EscMenu();
  
  void setVisible(bool);
  void render();
};

}

#endif
