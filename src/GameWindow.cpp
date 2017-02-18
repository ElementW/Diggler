#include "GameWindow.hpp"

#include <sstream>

#include <al.h>
#include <glm/detail/setup.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Game.hpp"
#include "GlobalProperties.hpp"
#include "GLFWHandler.hpp"
#include "GameState.hpp"
#include "MessageState.hpp"
#include "Audio.hpp"

namespace Diggler {

int GameWindow::InstanceCount = 0;

static void glfwErrorCallback(int error, const char *description) {
  getErrorStream() << "GLFW Error " << error << ": " << description << std::endl;
}

#ifdef DEBUG
static void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id,
  GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
  const char *sourceStr = "???";
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      sourceStr = "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      sourceStr = "WIN";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      sourceStr = "SHC";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      sourceStr = "3PT";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      sourceStr = "APP";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      sourceStr = "OTH";
      break;
  }
  const char *typeStr = "???";
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      typeStr = "ERR";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      typeStr = "DEP";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      typeStr = "UND";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      typeStr = "PRT";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      typeStr = "PRF";
      break;
    case GL_DEBUG_TYPE_MARKER:
      typeStr = "MKR";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      typeStr = "GP+";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      typeStr = "GP-";
      break;
    case GL_DEBUG_TYPE_OTHER:
      typeStr = "OTH";
      break;
  }
  const char *severityStr = "??";
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    severityStr = "HI";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    severityStr = "MD";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    severityStr = "LO";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    severityStr = "NT";
    break;
  }
  char line[3 + 1 + 3 + 1 + 2 + 2 + 1];
  std::snprintf(line, sizeof(line)/sizeof(line[0]), "%3s %3s %2s] ", sourceStr, typeStr, severityStr);
  getDebugStreamRaw() << line << message << std::endl;
}
#endif

GameWindow::GameWindow(Game *G) : G(G) {
  if (InstanceCount++ == 0) {
    glfwSetErrorCallback(glfwErrorCallback);
    int glfwStatus = glfwInit();
    if (glfwStatus != GL_TRUE) {
      std::ostringstream err;
      err << "GLFW init failed (" << glfwStatus << ')';
      throw std::runtime_error(err.str());
    }
    getOutputStreamRaw() << "GLFW linked " << GLFW_VERSION_MAJOR << '.' << GLFW_VERSION_MINOR <<
        '.' << GLFW_VERSION_REVISION << ", using " << glfwGetVersionString() << std::endl;
  }

  GLFWHandler::getInstance().setWindow(this, m_window);

  m_w = 640; m_h = 480;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API /*GLFW_OPENGL_ES_API*/);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 0); // Gimme aliasing everywhere
  //glfwWindowHint(GLFW_STENCIL_BITS, 8);

#ifdef DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

  m_window = glfwCreateWindow(m_w, m_h, "Diggler", nullptr, nullptr);
  if (!m_window) {
    glfwTerminate();
    throw std::runtime_error("GLFW window creation failed");
  }
  glfwMakeContextCurrent(m_window);
#ifdef DEBUG
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  glDebugMessageCallback(glDebugCallback, nullptr);
#endif
  glfwSetFramebufferSizeCallback(m_window, GLFWHandler::resize);
  glfwSetCursorPosCallback(m_window, GLFWHandler::cursorPos);
  glfwSetKeyCallback(m_window, GLFWHandler::key);
  glfwSetMouseButtonCallback(m_window, GLFWHandler::mouseButton);
  glfwSetScrollCallback(m_window, GLFWHandler::mouseScroll);
  glfwSetCharCallback(m_window, GLFWHandler::unichar);
  glfwSwapInterval(1);
  /*GLint bits;
  glGetIntegerv(GL_STENCIL_BITS, &bits);
  getDebugStream() << bits << " stencil bits" << std::endl;*/

  getOutputStreamRaw() << "GLM " << GLM_VERSION_MAJOR << '.' << GLM_VERSION_MINOR << '.' << GLM_VERSION_PATCH << ' '
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

  getOutputStreamRaw() << "-- Epoxy GL" << (epoxy_is_desktop_gl() ? "" : "ES") << epoxy_gl_version() << std::endl;

  if (InstanceCount == 1) { // If we're the first instance
    const uint8 *GL_version = glGetString(GL_VERSION);
    //const uint8 *GL_vendor = glGetString(GL_VENDOR);
    const uint8 *GL_renderer = glGetString(GL_RENDERER);
    getOutputStreamRaw() << "GL " << GL_version << " / " << GL_renderer << std::endl;
  }

  UIM = new UI::Manager;
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

GameWindow::~GameWindow() {
  m_currentState.reset();
  m_nextState.reset();
  delete UIM;

  glfwDestroyWindow(m_window);
  
  if (--InstanceCount == 0) {
    glfwTerminate();
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

void GameWindow::setNextState(std::unique_ptr<State> &&next) {
  m_nextState = std::move(next);
}

void GameWindow::run() {
  while (m_nextState != nullptr && !glfwWindowShouldClose(m_window)) {
    m_currentState = std::move(m_nextState);
    m_nextState = nullptr;
    UIM->clear();
    m_currentState->run();
    m_currentState.reset();
  }
}

void GameWindow::showMessage(const std::string &msg, const std::string &submsg) {
  setNextState(std::make_unique<MessageState>(this, msg, submsg));
}

}
