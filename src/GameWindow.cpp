#include "GameWindow.hpp"

#include <cctype>
#include <sstream>

#include "platform/BuildInfo.hpp"
#ifdef BUILDINFO_PLATFORM_MACOS
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Game.hpp"
#include "GlobalProperties.hpp"
#include "states/GameState.hpp"
#include "states/MessageState.hpp"
#include "Audio.hpp"
#include "ui/FontManager.hpp"
#include "ui/Manager.hpp"
#include "render/gl/Debug.hpp"
#include "util/Log.hpp"
#include "util/MemoryTracker.hpp"
#include "util/StringUtil.hpp"

namespace diggler {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "GameWindow";

int GameWindow::InstanceCount = 0;

static void glfwErrorCallback(int error, const char *description) {
  Log(Error, TAG) << "GLFW Error " << error << ": " << description;
}

void handleMouseButton(GLFWwindow *window, int key, int action, int mods) {
  auto win = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
  win->cbMouseButton(key, action, mods);
}

void handleCursorPos(GLFWwindow *window, double x, double y) {
  auto win = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
  win->cbCursorPos(x, y);
}

void handleMouseScroll(GLFWwindow *window, double x, double y) {
  auto win = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
  win->cbMouseScroll(x, y);
}

void handleKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
  auto win = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
  win->cbKey(key, scancode, action, mods);
}

void handleUnichar(GLFWwindow *window, unsigned int unichar) {
  auto win = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
  win->cbChar(static_cast<char32>(unichar));
}

void handleResize(GLFWwindow *window, int w, int h) {
  auto win = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
  win->cbResize(w, h);
}

GameWindow::GameWindow(Game *G) : G(G) {
  Util::MemoryTracker::ScopedCategory sc("GLFW");
  if (InstanceCount++ == 0) {
    glfwSetErrorCallback(glfwErrorCallback);
    int glfwStatus = glfwInit();
    if (glfwStatus != GLFW_TRUE) {
      std::ostringstream err;
      err << "GLFW init failed (" << glfwStatus << ')';
      throw std::runtime_error(err.str());
    }
    Log(Info, TAG) << "GLFW linked " << GLFW_VERSION_MAJOR << '.' << GLFW_VERSION_MINOR <<
        '.' << GLFW_VERSION_REVISION << ", using " << glfwGetVersionString();
  }

  m_w = 640; m_h = 480;

  if (GlobalProperties::GfxOverrides != nullptr) {
    for (const std::string &opt : Util::StringUtil::explode(GlobalProperties::GfxOverrides, ',')) {
      if (opt.size() == 4 && opt[0] == 'g' && opt[1] == 'l' &&
          isdigit(opt[2]) && isdigit(opt[3])) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opt[2] - '0');
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opt[3] - '0');
      } else if (opt.size() == 6 && opt[0] == 'g' && opt[1] == 'l' &&
          opt[2] == 'e' && opt[3] == 's' && isdigit(opt[4]) && isdigit(opt[5])) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opt[4] - '0');
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opt[5] - '0');
      } else if (opt == "fwd") {
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
      } else if (opt == "compat") {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
      } else if (opt == "core") {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      }
    }
  }

  glfwWindowHint(GLFW_SAMPLES, 0); // Gimme aliasing everywhere
  //glfwWindowHint(GLFW_STENCIL_BITS, 8);

#ifdef DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

  m_window = glfwCreateWindow(m_w, m_h, "Diggler", nullptr, nullptr);
  if (!m_window) {
    glfwTerminate();
    throw std::runtime_error("GLFW window creation failed");
  }
  glfwMakeContextCurrent(m_window);

  render::gl::OpenGL::init();

#ifdef DEBUG
  render::gl::Debug::enable();
#endif
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, handleResize);
  glfwSetCursorPosCallback(m_window, handleCursorPos);
  glfwSetKeyCallback(m_window, handleKey);
  glfwSetMouseButtonCallback(m_window, handleMouseButton);
  glfwSetScrollCallback(m_window, handleMouseScroll);
  glfwSetCharCallback(m_window, handleUnichar);
  glfwSwapInterval(1);
  /*GLint bits;
  glGetIntegerv(GL_STENCIL_BITS, &bits);
  getDebugStream() << bits << " stencil bits" << std::endl;*/

  Log(Info, TAG) << "GLM " << GLM_VERSION_MAJOR << '.' << GLM_VERSION_MINOR << '.' << GLM_VERSION_PATCH << ' '
#if GLM_ARCH & GLM_ARCH_AVX512_BIT
  << "AVX512 "
#endif
#if GLM_ARCH & GLM_ARCH_AVX2_BIT
  << "AVX2 "
#endif
#if GLM_ARCH & GLM_ARCH_AVX_BIT
  << "AVX "
#endif
#if GLM_ARCH & GLM_ARCH_SSE42_BIT
 << "SSE42 "
#endif
#if GLM_ARCH & GLM_ARCH_SSE41_BIT
  << "SSE41 "
#endif
#if GLM_ARCH & GLM_ARCH_SSSE3_BIT
 << "SSSE3 "
#endif
#if GLM_ARCH & GLM_ARCH_SSE3_BIT
  << "SSE3 "
#endif
#if GLM_ARCH & GLM_ARCH_SSE2_BIT
  << "SSE2 "
#endif
  ;

  auto glver = render::gl::OpenGL::version();
  Log(Info, TAG) << render::gl::OpenGL::loaderName() << ' ' <<
                    render::gl::OpenGL::loaderVersion() << " -- GL" <<
                    (glver.isGLES ? "ES" : "") << ' ' <<
                    glver.major << '.' << glver.minor <<
                    (glver.isCore ? " Core" : "") <<
                    (glver.isForward ? " FC" : "");

  if (InstanceCount == 1) { // If we're the first instance
    const char *GL_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    //const uint8 *GL_vendor = glGetString(GL_VENDOR);
    const char *GL_renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    Log(Info, TAG) << "GL " << GL_version << " / " << GL_renderer;
  }

  { Util::MemoryTracker::ScopedCategory sc(nullptr);
    UIM = new ui::Manager;
    UIM->onResize(m_w, m_h);

    G->init();
    UIM->setup(G);
    G->GW = this;
    G->UIM = UIM;
    G->A->loadSoundAssets();

    G->FM->loadFont(getAssetPath("04b08.png"), "04b08");
    G->FM->setDefaultFont("04b08");

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

GameWindow::~GameWindow() {
  m_currentState.reset();
  m_nextState.reset();
  delete UIM;

  { Util::MemoryTracker::ScopedCategory sc("GLFW");
    glfwDestroyWindow(m_window);

    if (--InstanceCount == 0) {
      glfwTerminate();
    }
  }
}

bool GameWindow::shouldClose() const {
  return glfwWindowShouldClose(m_window);
}

void GameWindow::setVisible(bool visible) {
  return visible ? glfwShowWindow(m_window) : glfwHideWindow(m_window);
}

bool GameWindow::isVisible() const {
  return glfwGetWindowAttrib(m_window, GLFW_VISIBLE);
}

void GameWindow::cbChar(char32 unichar) {
  UIM->onChar(unichar);
  m_currentState->onChar(unichar);
}

void GameWindow::cbKey(int key, int scancode, int action, int mods) {
  UIM->onKey(key, scancode, action, mods);
  m_currentState->onKey(key, scancode, action, mods);
}

void GameWindow::cbMouseButton(int key, int action, int mods) {
  UIM->onMouseButton(key, action, mods);
  m_currentState->onMouseButton(key, action, mods);
}

void GameWindow::cbCursorPos(double x, double y) {
  UIM->onCursorPos(x, y);
  m_currentState->onCursorPos(x, y);
}

void GameWindow::cbMouseScroll(double x, double y) {
  UIM->onMouseScroll(x, y);
  m_currentState->onMouseScroll(x, y);
}

void GameWindow::cbResize(int w, int h) {
  m_w = w; m_h = h;
  glViewport(0, 0, w, h);
  UIM->onResize(w, h);
  m_currentState->onResize(w, h);
}

states::State& GameWindow::state() const {
  return *m_currentState;
}

void GameWindow::setNextState(std::unique_ptr<states::State> &&next) {
  m_nextState = std::move(next);
}

void GameWindow::run() {
  while (m_nextState != nullptr && !glfwWindowShouldClose(m_window)) {
    m_currentState = std::move(m_nextState);
    m_nextState = nullptr;
    m_currentState->onCreate();
    m_currentState->onStart();
    m_currentState->m_runTime = glfwGetTime();
    while (m_nextState == nullptr && !glfwWindowShouldClose(m_window)) {
      double newTime = glfwGetTime();
      m_currentState->m_deltaTime = newTime - m_currentState->m_runTime;
      m_currentState->m_runTime = newTime;
      glfwPollEvents();
      m_currentState->onLogicTick();
      m_currentState->onFrameTick();
      glfwSwapBuffers(m_window);
    }
    m_currentState->onStop();
    m_currentState->onDestroy();
    m_currentState.reset();
  }
}

void GameWindow::showMessage(const std::string &msg, const std::string &submsg) {
  setNextState(std::make_unique<states::MessageState>(this, msg, submsg));
}

}
