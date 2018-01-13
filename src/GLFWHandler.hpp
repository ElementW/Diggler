#ifndef GLFW_HANDLER
#define GLFW_HANDLER

struct GLFWwindow;

namespace diggler {

class GameWindow;

/// C++ wrapper for C-style GLFWwindow callbacks
/// Currently handles one window, but could be modified to handle multiple
/// (using a GLFWwindow* <-> Window instance map)
class GLFWHandler {
private:
  GameWindow *win;
  void mouseButtonImpl(GLFWwindow *window, int key, int action, int mods);
  void cursorPosImpl(GLFWwindow *window, double x, double y);
  void mouseScrollImpl(GLFWwindow *window, double x, double y);
  void keyImpl(GLFWwindow *window, int key, int scancode, int action, int mods);
  void unicharImpl(GLFWwindow *window, unsigned int unichar);
  void resizeImpl(GLFWwindow *window, int w, int h);

public:
  ///
  /// @returns GLFWHandler singleton
  ///
  static GLFWHandler& getInstance() {
    static GLFWHandler instance;
    return instance;
  }
  
  static void setWindow(GameWindow *win, GLFWwindow *window) {
    getInstance().win = win;
  }
  
  static void mouseButton(GLFWwindow *window, int key, int action, int mods) {
    getInstance().mouseButtonImpl(window, key, action, mods); 
  }
  
  static void cursorPos(GLFWwindow *window, double x, double y) {
    getInstance().cursorPosImpl(window, x, y);
  }
  
  static void mouseScroll(GLFWwindow *window, double x, double y) {
    getInstance().mouseScrollImpl(window, x, y);
  }
  
  static void key(GLFWwindow *window, int key, int scancode, int action, int mods) {
    getInstance().keyImpl(window, key, scancode, action, mods);
  }
  
  static void unichar(GLFWwindow *window, unsigned int unichar) {
    getInstance().unicharImpl(window, unichar);
  }
  
  static void resize(GLFWwindow *window, int w, int h) {
    getInstance().resizeImpl(window, w, h);
  }

private:
  GLFWHandler() {}
  GLFWHandler(const GLFWHandler&); // prevent copies
    void operator=(const GLFWHandler&);
};

}

#endif