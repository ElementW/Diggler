#include "GameState.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iomanip>
#include <memory>
#include <sstream>
#include <thread>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Audio.hpp"
#include "../CaveGenerator.hpp"
#include "../Chatbox.hpp"
#include "../Clouds.hpp"
#include "../content/Registry.hpp"
#include "../content/texture/TextureLoader.hpp"
#include "../EscMenu.hpp"
#include "../Game.hpp"
#include "../GlobalProperties.hpp"
#include "../KeyBinds.hpp"
#include "../LocalPlayer.hpp"
#include "../network/msgtypes/BlockUpdate.hpp"
#include "../network/msgtypes/PlayerJoin.hpp"
#include "../network/msgtypes/PlayerUpdate.hpp"
#include "../network/NetHelper.hpp"
#include "../Particles.hpp"
#include "../render/gl/FBO.hpp"
#include "../render/gl/ProgramManager.hpp"
#include "../render/Renderer.hpp"
#include "../Skybox.hpp"
#include "../ui/FontManager.hpp"
#include "../ui/Manager.hpp"
#include "../util/MemoryTracker.hpp"

using std::unique_ptr;

namespace diggler {
namespace states {

GameState::GameState(GameWindow *GW) :
  GW(GW),
  CMH(*this),
  bloom(*GW->G) {
  G = GW->G;
  int w = GW->getW(),
      h = GW->getH();

  // Initialized in setupUI
  m_chatBox = nullptr;

  float coords[6*3*2*3] = {
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f, 
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    
    1.0f,-1.0f, 1.0f, 
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f, 
    1.0f, 1.0f,-1.0f, 
    1.0f,-1.0f,-1.0f, 
    -1.0f,-1.0f,-1.0f,
    
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f, 
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f, 
    1.0f, 1.0f, 1.0f, 
    1.0f,-1.0f,-1.0f, 
    1.0f, 1.0f,-1.0f, 
    
    1.0f,-1.0f,-1.0f, 
    1.0f, 1.0f, 1.0f, 
    1.0f,-1.0f, 1.0f, 
    1.0f, 1.0f, 1.0f, 
    1.0f, 1.0f,-1.0f, 
    -1.0f, 1.0f,-1.0f,
    
    1.0f, 1.0f, 1.0f, 
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f, 
  };
  m_highlightBox.vbo.setData(coords, 6*3*2*3);
  m_highlightBox.program = G->PM->getProgram("3d");
  m_highlightBox.att_coord = m_highlightBox.program->att("coord");
  m_highlightBox.uni_unicolor = m_highlightBox.program->uni("unicolor");
  m_highlightBox.uni_mvp = m_highlightBox.program->uni("mvp");
  { render::gl::VAO::Config cfg = m_highlightBox.vao.configure();
    cfg.vertexAttrib(m_highlightBox.vbo, m_highlightBox.att_coord, 3, GL_FLOAT, 0);
    cfg.commit();
  }

  m_3dFbo = new render::gl::FBO(w, h, PixelFormat::RGB, true);
  m_3dFbo->tex->setWrapping(Texture::Wrapping::ClampEdge);
  m_3dRenderVBO = new render::gl::VBO();
  m_clouds = new Clouds(G, 32, 32, 4);
  //m_sky = new Skybox(G, getAssetPath("alpine"));
  m_3dFboRenderer = G->PM->getProgram("2d", "texture0", "texcoord0"); //getSpecialProgram("effect3dRender");
  m_3dFboRenderer_coord = m_3dFboRenderer->att("coord");
  m_3dFboRenderer_texcoord = m_3dFboRenderer->att("texcoord");
  m_3dFboRenderer_mvp = m_3dFboRenderer->uni("mvp");

  Coord2DTex renderQuad[6] = {
    {0, 0, 0, 0},
    {1, 0, 1, 0},
    {0, 1, 0, 1},

    {1, 1, 1, 1},
    {0, 1, 0, 1},
    {1, 0, 1, 0}
  };
  m_3dRenderVBO->setData(renderQuad, 6*sizeof(Coord2DTex));

  m_crossHair.tex = content::Texture::TextureLoader::load(*G, content::Image::ImageFormats::PNG,
      getAssetPath("crosshair.png"), PixelFormat::RGBA)->texture;

  //"\f0H\f1e\f2l\f3l\f4l\f5o \f6d\f7e\f8m\f9b\faa\fbz\fcz\fde\fes\ff,\n\f0ye see,it werks purrfektly :D\n(and also; it's optimized)"

  debugInfo.show = false;

  m_mouseLocked = false;
  nextNetUpdate = 0;
}

GameState::Bloom::Bloom(Game &G) {
  enable = true;
  scale = 4;

  extractor.fbo = new render::gl::FBO(G.GW->getW()/scale, G.GW->getH()/scale, PixelFormat::RGBA);
  extractor.fbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
  extractor.fbo->tex->setWrapping(Texture::Wrapping::ClampEdge);
  extractor.prog = G.PM->getProgram("bloomExtractor");
  extractor.att_coord = extractor.prog->att("coord");
  extractor.att_texcoord = extractor.prog->att("texcoord");
  extractor.uni_mvp = extractor.prog->uni("mvp");

  renderer.fbo = new render::gl::FBO(G.GW->getW()/scale, G.GW->getH()/scale, PixelFormat::RGBA);
  renderer.fbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
  renderer.fbo->tex->setWrapping(Texture::Wrapping::ClampEdge);
  renderer.prog = G.PM->getProgram("bloom");
  renderer.att_coord = renderer.prog->att("coord");
  renderer.att_texcoord = renderer.prog->att("texcoord");
  renderer.uni_mvp = renderer.prog->uni("mvp");
  renderer.uni_pixshift = renderer.prog->uni("pixshift");
}

GameState::Bloom::~Bloom() {
  delete extractor.fbo;
  delete renderer.fbo;
}

void GameState::setupUI() {
  UI.FPS = G->UIM->add<ui::Text>();
  UI.FPS->setUpdateFrequencyHint(render::FontRendererTextBufferUsage::Dynamic);
  UI.FPS->setScale(2, 2);

  UI.DebugInfo = G->UIM->add<ui::Text>();
  UI.DebugInfo->setUpdateFrequencyHint(render::FontRendererTextBufferUsage::Stream);
  UI.DebugInfo->setVisible(false);

  UI.EM = G->UIM->add<EscMenu>();
  UI.EM->setVisible(false);

  m_chatBox = new Chatbox(G);

  updateViewport();
}

GameState::~GameState() {
  delete m_clouds;
  delete m_chatBox;
  //delete m_sky;
}

void GameState::onChar(char32 unichar) {
  if (m_chatIgnFirstKey) {
    m_chatIgnFirstKey = false;
    return;
  }
  if (m_chatBox->isChatting()) {
    m_chatBox->handleChar(unichar);
  }
}

void GameState::onKey(int key, int scancode, int action, int mods) {
  if (!(action == GLFW_PRESS || action == GLFW_RELEASE))
    return;

  switch (key) {
  case GLFW_KEY_ESCAPE:
    if (mods & GLFW_MOD_SHIFT)
      glfwSetWindowShouldClose(*G->GW, true);
    break;
  case GLFW_KEY_F1:
    if (action == GLFW_PRESS)
      bloom.enable = !bloom.enable;
    break;
  case GLFW_KEY_F5:
    if (action == GLFW_PRESS) {
      debugInfo.show = !debugInfo.show;
      UI.DebugInfo->setVisible(debugInfo.show);
    }
    break;
  case GLFW_KEY_F6:
    if (action == GLFW_PRESS) {
      unlockMouse();
    }
    break;
  case GLFW_KEY_F7:
    if (action == GLFW_PRESS) {
      G->RP->wavingLiquids = !G->RP->wavingLiquids;
      // TODO: better way
      G->U->getWorld(0)->onRenderPropertiesChanged();
    }
    break;
  case GLFW_KEY_F8:
    if (action == GLFW_PRESS) {
      G->U->getWorld(0)->refresh();
    }
    break;
  case GLFW_KEY_F12:
    if (action == GLFW_PRESS && (mods & GLFW_MOD_SHIFT)) {
      ::abort();
    }
    break;
  default:
    break;
  }

  if (m_chatBox && m_chatBox->isChatting()) {
    switch (key) {
    case GLFW_KEY_ENTER:
      if (action == GLFW_PRESS) {
        std::string str = m_chatBox->getChatString();
        if (str.size() > 0) {
          NetHelper::SendChat(G, str);
        }
        m_chatBox->setIsChatting(false);
      }
      break;
    case GLFW_KEY_ESCAPE:
      if (action == GLFW_PRESS)
        m_chatBox->setIsChatting(false);
      break;
    default:
      m_chatBox->handleKey(key, scancode, action, mods);
      break;
    }
  } else {
    bool isMenuToggled = UI.EM->isVisible();
    if (key == G->KB->gameMenu && action == GLFW_PRESS) {
      isMenuToggled = !isMenuToggled;
      UI.EM->setVisible(isMenuToggled);
      if (isMenuToggled) {
        unlockMouse();
        G->LP->goForward(false);
        G->LP->goBackward(false);
        G->LP->goLeft(false);
        G->LP->goRight(false);
      } else {
        lockMouse();
      }
    }
    if (!isMenuToggled) {
      if (key == G->KB->forward) {
        G->LP->goForward(action == GLFW_PRESS);
      } else if (key == G->KB->backward) {
        G->LP->goBackward(action == GLFW_PRESS);
      } else if (key == G->KB->left) {
        G->LP->goLeft(action == GLFW_PRESS);
      } else if (key == G->KB->right) {
        G->LP->goRight(action == GLFW_PRESS);
      } else if (key == G->KB->jump) {
        if (action == GLFW_PRESS)
          G->LP->jump();
      } else if (key == G->KB->chat) {
        if (action == GLFW_PRESS) {
          m_chatBox->setIsChatting(true);
          m_chatIgnFirstKey = true;
        }
      } else if (key == GLFW_KEY_V) {
        G->LP->setHasNoclip(true);
      } else if (key == GLFW_KEY_B) {
        G->LP->setHasNoclip(false);
      }
    }
  }
}

void GameState::lockMouse() {
  glfwSetInputMode(*GW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  double x, y;
  glfwGetCursorPos(*GW, &x, &y);
  cX = static_cast<int>(x); cY = static_cast<int>(y);
  m_mouseLocked = true;
}

void GameState::unlockMouse() {
  m_mouseLocked = false;
  glfwSetInputMode(*GW, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


void GameState::onMouseButton(int key, int action, int mods) {
  (void) mods;

  if (!m_mouseLocked && action == GLFW_PRESS && !UI.EM->isVisible()) {
    lockMouse();
  }

  if (action == GLFW_PRESS) {
    glm::ivec3 pointed, face;
    if (G->LP->raytracePointed(32, &pointed, &face)) {
      net::OutMessage msg;
      if (key == GLFW_MOUSE_BUTTON_LEFT) {
        net::MsgTypes::BlockUpdateBreak bub;
        bub.worldId = G->LP->W->id;
        bub.pos = pointed;
        bub.writeToMsg(msg);
      } else {
        net::MsgTypes::BlockUpdatePlace bup;
        bup.worldId = G->LP->W->id;
        bup.pos = face;
        bup.id = 2; //content::BlockUnknownId;
        bup.data = 0;
        bup.writeToMsg(msg);
      }
      sendMsg(msg, net::Tfer::Rel, net::Channels::MapUpdate);
    }
  }
}

void GameState::onCursorPos(double x, double y) {
  if (!m_mouseLocked) {
    return;
  }
  int cx = static_cast<int>(x), dx = cx-cX, cy = static_cast<int>(y), dy = cy-cY;
  const float mousespeed = 0.003f;

  angles.x -= dx * mousespeed;
  angles.y -= dy * mousespeed;

  if(angles.x < -M_PI)
    angles.x += M_PI * 2;
  else if(angles.x > M_PI)
    angles.x -= M_PI * 2;

  if(angles.y < -M_PI / 2)
    angles.y = -M_PI / 2 + 0.001;
  if(angles.y > M_PI / 2)
    angles.y = M_PI / 2 - 0.001;

  G->LP->angle = angles.x;
  lookat.x = sinf(angles.x) * cosf(angles.y);
  lookat.y = sinf(angles.y);
  lookat.z = cosf(angles.x) * cosf(angles.y);

  G->LP->lookAt(lookat);

  cX = cx; cY = cy;
}

void GameState::onResize(int w, int h) {
  updateViewport();
}

void GameState::onMouseScroll(double x, double y) {
  (void) x; (void) y;
}

void GameState::updateViewport() {
  int w = GW->getW(), h = GW->getH();
  ui::Manager &UIM = *G->UIM;
  G->LP->camera.setPersp((float)M_PI/180*75.0f, (float)w / h, 0.1f, 32.0f);
  m_3dFbo->resize(w, h);
  bloom.extractor.fbo->resize(w/bloom.scale, h/bloom.scale);
  //bloom.extractor.fbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
  bloom.renderer.fbo->resize(w/bloom.scale, h/bloom.scale);
  //bloom.renderer.fb->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);

  { int tw = 5*UIM.scale, th = 5*UIM.scale;
    m_crossHair.mat = glm::scale(glm::translate(*UIM.PM,
      glm::vec3((w-tw)/2, (h-th)/2, 0)),
      glm::vec3(tw, tw, 0));
  }

  int lineHeight = G->FM->getDefaultFont()->getHeight()*UIM.scale;

  UI.FPS->setPos(16, 16);
  UI.DebugInfo->setPos(0, h-(lineHeight+UI.DebugInfo->getSize().y));
  const int menuWidth = G->UIM->scale*128;
  UI.EM->setArea(ui::Element::Area(w - menuWidth, 0, menuWidth, h));

  m_chatBox->setPosition(4, 64);
  updateUI();
}

void GameState::sendMsg(net::OutMessage &msg, net::Tfer mode, net::Channels chan) {
  G->H.send(*G->NS, msg, mode, chan);
}

void GameState::onStart() {
  setupUI();

  fpsCounter = 0;
  fpsNextSampling = 1.;

  LocalPlayer *LP = G->LP;
  LP->position = glm::vec3(-2, 2, -2);
  angles.x = M_PI/4; angles.y = M_PI/4;
  lookat.x = sinf(angles.x) * cosf(angles.y);
  lookat.y = sinf(angles.y);
  lookat.z = cosf(angles.x) * cosf(angles.y);
  LP->lookAt(lookat);
  LP->forceCameraUpdate();
  G->A->update();
  LP->setHasNoclip(true);
}

void GameState::onLogicTick() {
  if (!processNetwork()) return;

  G->updateTime(runTime());
  if (runTime() > fpsNextSampling) {
    char str[10]; std::snprintf(str, 10, "FPS: %-4d", fpsCounter);
    UI.FPS->setText(std::string(str));
    fpsNextSampling = runTime() + 1;
    fpsCounter = 0;
  }
}

void GameState::onFrameTick() {
  std::chrono::time_point<std::chrono::steady_clock> frameStart, frameEnd;

  G->R->beginFrame();
  frameStart = std::chrono::steady_clock::now();

  LocalPlayer *LP = G->LP;
  double T = G->Time;
  if (runTime() > nextNetUpdate) {
    net::MsgTypes::PlayerUpdateMove pum;
    pum.position = LP->position;
    if (LP->velocity != glm::vec3()) {
      pum.velocity = LP->velocity;
      pum.accel = LP->accel;
    }
    pum.angle = LP->angle;
    net::OutMessage msg; pum.writeToMsg(msg);
    sendMsg(msg, net::Tfer::Unrel, net::Channels::Movement);
    nextNetUpdate = T+1.0/G->PlayerPosUpdateFreq;
  }

  if (bloom.enable) {
    m_3dFbo->bind();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  double deltaT = deltaTime();
  LP->update(deltaT);

  glm::mat4 m_transform = LP->getPVMatrix();

  /*** 3D PART ***/
  // TODO: multiworld
  WorldRef WR = G->U->getWorld(0);
  World &W = *WR;
  /*glm::mat4 cloudmat = glm::scale(glm::translate(m_transform, glm::vec3(0.f, W.cloudsHeight, 0.f)), glm::vec3(4*CX, 1, 4*CZ));
  m_clouds->render(cloudmat);*/

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  render::RenderParams rp;
  rp.world = WR.get();
  rp.transform = m_transform;
  rp.frustum = G->LP->camera.frustum;
  G->R->renderers.world->render(rp);
  for (Player &p : G->players) {
    p.update(deltaT);
    if (G->LP->camera.frustum.sphereInFrustum(p.position, 2))
      p.render(m_transform);
  }
  //W.renderTransparent(m_transform);

  /*static ParticleEmitter pe(G);
  pe.posAmpl = glm::vec3(1, 1, 1);
  pe.pTemplate.color = glm::vec4(0, 0.4, 0.9, 1);
  pe.pTemplate.size = 0.07;
  pe.velAmpl = glm::vec3(0, 1, 0);
  pe.pTemplate.accel = glm::vec3(0, -.7, 0);
  pe.pTemplate.decay = 4;
  pe.decayAmpl = 2;

  pe.setMaxCount(4*4*800);
  pe.posAmpl = glm::vec3(2*16, 1, 2*16);
  pe.pos = glm::vec3(2*16, 4*16+4, 2*16);
  pe.pTemplate.vel = glm::vec3(0, -4, 0);
  pe.velAmpl = glm::vec3(0, 2, 0); rain

  pe.update(deltaT);
  render::RenderParams rp;
  rp.transform = m_transform;
  G->R->PR->render(rp);*/

  // TODO: replace harcoded 32 viewdistance
  if (G->LP->raytracePointed(32, &m_pointedBlock, &m_pointedFacing)) {
    m_highlightBox.program->bind();
    m_highlightBox.vao.bind();

    glUniform4f(m_highlightBox.uni_unicolor, 1.f, 1.f, 1.f, .1f);
    glUniformMatrix4fv(m_highlightBox.uni_mvp, 1, GL_FALSE, glm::value_ptr(
      glm::scale(glm::translate(m_transform, glm::vec3(m_pointedBlock)+glm::vec3(.5f)), glm::vec3(0.5f*1.03f))));
    glDrawArrays(GL_TRIANGLES, 0, 6*2*3);

    m_highlightBox.vao.unbind();
  }

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  G->LP->render(m_transform);

  if (bloom.enable) {
    m_3dFbo->unbind();
    G->UIM->drawFullTexV(*m_3dFbo->tex);

    m_3dFbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
    bloom.extractor.fbo->bind();
    glViewport(0, 0, GW->getW()/bloom.scale, GW->getH()/bloom.scale);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!bloom.vao) {
      bloom.vao = std::make_unique<render::gl::VAO>();
      render::gl::VAO::Config cfg = bloom.vao->configure();
      cfg.vertexAttrib(*m_3dRenderVBO, bloom.extractor.att_coord, 2, GL_SHORT, sizeof(Coord2DTex), 0);
      cfg.vertexAttrib(*m_3dRenderVBO, bloom.extractor.att_texcoord, 2, GL_BYTE, sizeof(Coord2DTex), offsetof(Coord2DTex, u));
      cfg.commit();
    }
    bloom.extractor.prog->bind();
    bloom.vao->bind();
    glUniformMatrix4fv(bloom.extractor.uni_mvp, 1, GL_FALSE, glm::value_ptr(*G->UIM->PM1));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_3dFbo->tex->setFiltering(Texture::Filter::Nearest, Texture::Filter::Nearest);

    bloom.renderer.fbo->bind();
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bloom.extractor.fbo->tex->bind();
    bloom.renderer.prog->bind();
    glUniformMatrix4fv(bloom.renderer.uni_mvp, 1, GL_FALSE, glm::value_ptr(*G->UIM->PM1));
    glUniform2f(bloom.renderer.uni_pixshift, 1.f/(GW->getW()/bloom.scale), 1.f/(GW->getH()/bloom.scale));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    bloom.renderer.fbo->unbind();

    // render to real surface
    glViewport(0, 0, GW->getW(), GW->getH());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    bloom.extractor.fbo->tex->bind();
    bloom.extractor.fbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    bloom.vao->unbind();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  /*** 2D PART ***/
  G->UIM->drawFullRect(glm::vec4(1.f, 0.f, 0.f, 1-G->LP->health));
  updateUI();
  drawUI();

  G->R->endFrame();
  frameEnd = std::chrono::steady_clock::now();
  frameTime = std::chrono::duration_cast<std::chrono::duration<uint64, std::micro>>(frameEnd  - frameStart).count();

  fpsCounter++;
}

void GameState::onStop() {
  net::OutMessage quit(net::MessageType::PlayerQuit);
  sendMsg(quit, net::Tfer::Rel);
}

void GameState::gameLoop() {
  double lastT, deltaT, T, fpsT = 0; int frames = 0;
  LocalPlayer *LP = G->LP;
}

void GameState::renderDeathScreen() {
  double red = std::max(1-(G->Time-G->LP->deathTime), 0.0);
  glClearColor(red, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameState::updateUI() {
  LocalPlayer &LP = *G->LP;
  if (debugInfo.show) {
    WorldRef w;
    int chunkMem = 0, maxChunkMem = 0;
    for (const std::pair<int, WorldWeakRef> &wr : *G->U)
      if ((w = wr.second.lock()))
        for (std::pair<const glm::ivec3, ChunkWeakRef> &cr : *w) {
          ChunkRef c(cr.second.lock());
          if (c) {
            chunkMem += c->blkMem;
            maxChunkMem += Chunk::AllocaSize;
          }
        }
    constexpr auto CX = Chunk::CX, CY = Chunk::CY, CZ = Chunk::CZ;
    std::ostringstream oss;
    oss << std::setprecision(3) <<
      "HP: " << LP.health << std::endl <<
      "x: " << LP.position.x << std::endl <<
      "y: " << LP.position.y << std::endl <<
      "z: " << LP.position.z << std::endl <<
      "vy: " << LP.velocity.y << std::endl <<
      "r: " << LP.angle << std::endl <<
      // TODO reintroduce "chunk tris: " << lastVertCount / 3 << std::endl <<
      "chunk mem: " << chunkMem / 1024 << " kib / " << (chunkMem*100/maxChunkMem) << '%' <<
        std::endl <<
      "Pointing at: " << LP.W->getBlockId(m_pointedBlock.x, m_pointedBlock.y, m_pointedBlock.z) <<
        " @ " << m_pointedBlock.x << ' ' << m_pointedBlock.y << ' ' << m_pointedBlock.z <<
      " C: " << divrd(m_pointedBlock.x, CX) << ' ' << divrd(m_pointedBlock.y, CY) << ' ' <<
        divrd(m_pointedBlock.z, CZ) << std::endl <<
      "RX: " << G->H.getRxBytes() << std::endl <<
      "TX: " << G->H.getTxBytes() << std::endl <<
      "Frame time: " << frameTime;
    oss << std::endl;
    for (auto stats : Util::MemoryTracker::categoryStats()) {
      oss << stats.name << ' ' << stats.bytesUsed << std::endl;
    }
    UI.DebugInfo->setText(oss.str());
  }
}

void GameState::drawUI() {
  G->UIM->render();
  m_chatBox->render();

  G->UIM->drawTex(m_crossHair.mat, *m_crossHair.tex);
  // TODO render weapon

  G->UIM->drawTex(*G->UIM->PM, ui::Element::Area{0,0,128,128}, *G->CR->getAtlas());
}

bool GameState::processNetwork() {
  while (G->H.recv(m_msg, 0)) {
    if (!CMH.handleMessage(m_msg)) {
      return false;
    }
  }
  return true;
}

}
}
