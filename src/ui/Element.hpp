#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP

#include "../Platform.hpp"

#include "../platform/types/mat4.hpp"

namespace diggler {

class Game;

namespace ui {

class Manager;

class Element {
  friend Manager;

public:
  struct Point {
    int x, y;
  };
  struct Area {
    int x, y, w, h;
    Area() : x(0), y(0), w(0), h(0) {}
    Area(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
    bool isIn(int x, int y) const;
    inline bool isIn(const Point &p) const { return isIn(p.x, p.y); }
  };
  struct Coord {
    int x, y;
    uint8 r, g, b, a;
    float u, v;
  };
  struct Color {
    float r, g, b, a;
  };

private:
  bool m_isCursorOver, m_hasFocus, m_isVisible;
  bool m_manualRender, m_manualInput;
  Area m_renderArea, m_inputArea;

protected:
  const Game *G;
  class Manager *M;

public:
  enum class MouseButton {
    Unknown,
    Left,
    Middle,
    Right
  };

  Element(Manager*);
  virtual ~Element();

  void setManual(bool render = true, bool input = true);

  const Area& renderArea() const { return m_renderArea; }
  virtual void onRenderAreaChange(const Area &newArea);
  virtual void onRenderAreaChanged();
  void setRenderArea(const Area &area) { onRenderAreaChange(area); m_renderArea = area; onRenderAreaChanged(); }

  const Area& inputArea() const { return m_inputArea; }
  virtual void onInputAreaChange(const Area &newArea);
  virtual void onInputAreaChanged();
  void setInputArea(const Area &area) { onInputAreaChange(area); m_inputArea = area; onInputAreaChanged(); }

  void setArea(const Area &area) { setRenderArea(area); setInputArea(area); }
  void setArea(int x, int y, int w, int h) { setArea(Area { x, y, w, h }); }
  void setPos(int x, int y) { setArea(Area { x, y, 1, 1 }); }

  virtual bool focusPassesThrough() { return false; }
  bool isFocused() const { return m_hasFocus; }
  virtual void onFocus();
  virtual void onFocusLost();

  virtual bool cursorPassesThrough(int x, int y) { (void)x; (void)y; return false; }
  bool isCursorOver() const { return m_isCursorOver; }
  virtual void onMouseDown(int x, int y, MouseButton button);
  virtual void onMouseUp(int x, int y, MouseButton button);
  virtual void onMouseScroll(double x, double y);
  virtual void onCursorMove(int x, int y);
  virtual void onCursorEnter(int x, int y);
  virtual void onCursorLeave(int x, int y);

  virtual void setVisible(bool v) { m_isVisible = v; }
  bool isVisible() const { return m_isVisible; }

  virtual void render(const mat4&) const = 0;
  void render() const;
};

}
}

#endif
