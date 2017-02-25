#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "State.hpp"

#include <thread>
#include <map>
#include <list>

#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec.hpp>

#include "GameWindow.hpp"
#include "render/gl/FBO.hpp"
#include "render/gl/VBO.hpp"
#include "render/gl/VAO.hpp"
#include "content/Content.hpp"
#include "network/Network.hpp"
#include "network/ClientMessageHandler.hpp"
// TODO strip?
#include "Chunk.hpp"

namespace Diggler {

class Skybox;
class KeyBindings;
class Font;
class Clouds;
class Game;
class Chatbox;
namespace Render {
namespace gl {
class Program;
}
}
namespace UI {
class Text;
}

class GameState : public State {
public:
  GameWindow *GW;
  Game *G;

private:
  Net::ClientMessageHandler CMH;

  std::string m_serverHost;
  int m_serverPort;

  glm::vec3 m_lookat;
  glm::vec2 m_angles;

  Render::gl::FBO *m_3dFbo;
  const Render::gl::Program *m_3dFboRenderer;
  GLuint m_3dFboRenderer_coord, m_3dFboRenderer_texcoord, m_3dFboRenderer_mvp;

  struct Bloom {
    bool enable;
    int scale;
    std::unique_ptr<Render::gl::VAO> vao;
    struct Extractor {
      Render::gl::FBO *fbo;
      const Render::gl::Program *prog;
      GLuint att_coord, att_texcoord, uni_mvp;
    } extractor;
    struct Renderer {
      Render::gl::FBO *fbo;
      const Render::gl::Program *prog;
      GLuint att_coord, att_texcoord, uni_pixshift, uni_mvp;
    } renderer;
    Bloom(Game&);
    ~Bloom();
  } bloom;

  Render::gl::VBO *m_3dRenderVBO;
  struct Coord2DTex { int16 x, y; uint8 u, v; };
  Clouds *m_clouds;
  Skybox *m_sky;

  struct CrossHair {
    Texture *tex;
    glm::mat4 mat;
  } m_crossHair;

  struct {
    Render::gl::VBO vbo;
    Render::gl::VAO vao;
    const Render::gl::Program *program;
    GLuint att_coord, uni_unicolor, uni_mvp;
  } m_highlightBox;

  KeyBindings *m_keybinds;

public:
  Chatbox *m_chatBox;
private:
  bool m_chatIgnFirstKey;

  bool m_mouseLocked; int cX, cY;
  glm::vec3 angles, lookat;

  glm::ivec3 m_pointedBlock, m_pointedFacing;

  std::thread m_networkThread;

  Net::InMessage m_msg;
  float nextNetUpdate;

  struct ConnectingUI {
    UI::Text *Connecting;
    UI::Text *Dot;
  };

  bool isMenuToggled = false;

  struct {
    bool show;
  } debugInfo;

  struct {
    UI::Text *FPS;

    UI::Text *DebugInfo;

    class EscMenu *EM;
  } UI;

  uint64 frameTime;

  void setupUI();
  void lockMouse();
  void unlockMouse();

public:
  // TODO: REMOVEME!!!
  std::list<ChunkRef> holdChunksInMem;

public:
  GameState(GameWindow *W, const std::string &servHost, int servPort);
  ~GameState();

  void onMouseButton(int key, int action, int mods);
  void onCursorPos(double x, double y);
  void onMouseScroll(double x, double y);
  void onKey(int key, int scancode, int action, int mods);
  void onChar(char32 unichar);
  void onResize(int w, int h);
  void run();

  void updateViewport();

  bool connectLoop();
  bool teamSelectLoop();
  bool classSelectLoop();
  void gameLoop();

  void renderDeathScreen();
  void updateUI();
  void drawUI();
  bool processNetwork();

  void sendMsg(Net::OutMessage &msg, Net::Tfer mode, Net::Channels chan = Net::Channels::Base);
};

}

#endif
