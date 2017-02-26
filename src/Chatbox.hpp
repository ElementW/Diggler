#ifndef DIGGLER_CHATBOX_HPP
#define DIGGLER_CHATBOX_HPP

#include <chrono>
#include <list>
#include <memory>

#include <glm/glm.hpp>

#include "Platform.hpp"
#include "render/gl/VBO.hpp"
#include "ui/Text.hpp"

namespace Diggler {

namespace Render {
namespace gl {
class Program;
}
}

class Game;

class Chatbox {
public:
  using system_clock = std::chrono::system_clock;
  using time_point = std::chrono::time_point<system_clock>;

private:
  static const Render::gl::Program *RenderProgram;
  static GLint RenderProgram_coord, RenderProgram_color, RenderProgram_mvp;
  
  bool m_isChatting;
  Game *G;
  struct ChatEntry {
    time_point date;
    int height;
    std::shared_ptr<UI::Text> text;
    
    ~ChatEntry();
  };
  std::list<ChatEntry> m_chatEntries;
  // TODO: Update when libstdc++ supports locale codecvt facets
  //std::u32string m_chatString;
  std::string m_chatString;
  std::shared_ptr<UI::Text> m_chatText;
  struct Vertex {
    float x, y, r, g, b, a;
  };
  Render::gl::VBO m_vbo;
  int m_posX, m_posY;

public:
  Chatbox(Game *G);
  ~Chatbox();
  
  bool isChatting() const;
  void setIsChatting(bool value);
  
  void addChatEntry(const std::string& text);
  
  void handleChar(char32 unichar);
  void handleKey(int key, int scancode, int action, int mods);
  
  void setPosition(int x, int y);
  
  void render();
  
  std::string getChatString() const { return m_chatString; }
};

}

#endif /* DIGGLER_CHATBOX_HPP */
