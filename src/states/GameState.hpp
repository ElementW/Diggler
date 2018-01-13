#ifndef DIGGLER_STATES_GAME_STATE_HPP
#define DIGGLER_STATES_GAME_STATE_HPP
#include "State.hpp"

#include <thread>
#include <map>
#include <list>

#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec.hpp>

#include "../GameWindow.hpp"
#include "../render/gl/FBO.hpp"
#include "../render/gl/VBO.hpp"
#include "../render/gl/VAO.hpp"
#include "../content/Content.hpp"
#include "../network/Network.hpp"
#include "../network/ClientMessageHandler.hpp"
// TODO strip?
#include "../Chunk.hpp"

namespace diggler {

class EscMenu;
class Skybox;
class KeyBindings;
class Font;
class Clouds;
class Game;
class Chatbox;
namespace render {
namespace gl {
class Program;
}
}
namespace ui {
class Text;
}

namespace states {

class GameState : public State {
public:
  GameWindow *GW;
  Game *G;

private:
  net::ClientMessageHandler CMH;

  glm::vec3 m_lookat;
  glm::vec2 m_angles;

  render::gl::FBO *m_3dFbo;
  const render::gl::Program *m_3dFboRenderer;
  GLuint m_3dFboRenderer_coord, m_3dFboRenderer_texcoord, m_3dFboRenderer_mvp;

  struct Bloom {
    bool enable;
    int scale;
    std::unique_ptr<render::gl::VAO> vao;
    struct Extractor {
      render::gl::FBO *fbo;
      const render::gl::Program *prog;
      GLuint att_coord, att_texcoord, uni_mvp;
    } extractor;
    struct Renderer {
      render::gl::FBO *fbo;
      const render::gl::Program *prog;
      GLuint att_coord, att_texcoord, uni_pixshift, uni_mvp;
    } renderer;
    Bloom(Game&);
    ~Bloom();
  } bloom;

  render::gl::VBO *m_3dRenderVBO;
  struct Coord2DTex { int16 x, y; uint8 u, v; };
  Clouds *m_clouds;
  Skybox *m_sky;

  struct CrossHair {
    std::shared_ptr<Texture> tex;
    glm::mat4 mat;
  } m_crossHair;

  struct {
    render::gl::VBO vbo;
    render::gl::VAO vao;
    const render::gl::Program *program;
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

  net::InMessage m_msg;
  float nextNetUpdate;

  struct {
    bool show;
  } debugInfo;

  struct {
    std::shared_ptr<ui::Text> FPS;
    std::shared_ptr<ui::Text> DebugInfo;
    std::shared_ptr<EscMenu> EM;
  } UI;

  int fpsCounter;
  double fpsNextSampling;
  uint64 frameTime;

  void setupUI();
  void lockMouse();
  void unlockMouse();

public:
  // TODO: REMOVEME!!!
  std::list<ChunkRef> holdChunksInMem;

public:
  GameState(GameWindow *W);
  ~GameState();

  void onStart() override;
  void onLogicTick() override;
  void onFrameTick() override;
  void onStop() override;

  void onMouseButton(int key, int action, int mods) override;
  void onCursorPos(double x, double y) override;
  void onMouseScroll(double x, double y) override;
  void onKey(int key, int scancode, int action, int mods) override;
  void onChar(char32 unichar) override;
  void onResize(int w, int h) override;
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

  void sendMsg(net::OutMessage &msg, net::Tfer mode, net::Channels chan = net::Channels::Base);
};

}
}

#endif /* DIGGLER_STATES_GAME_STATE_HPP */
