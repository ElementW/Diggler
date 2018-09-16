#ifndef UI_BUTTON_HPP
#define UI_BUTTON_HPP

#include "Element.hpp"

#include <memory>

namespace diggler {
namespace ui {

class Text;

class Button : public Element {
private:
  std::shared_ptr<Text> m_text;
  std::string m_label;
  Color *m_displayedColor;
  void update();

public:
  Color color, colorHover, colorPress;

  Button(Manager*, const Area &area, const std::string &label = "<?>");

  void setLabel(const std::string &label);
  std::string getLabel() const;

  void onRenderAreaChange(const Area &newArea) override;
  void onMouseDown(int x, int y, MouseButton button) override;
  void onMouseUp(int x, int y, MouseButton button) override;
  void onCursorEnter(int x, int y) override;
  void onCursorLeave(int x, int y) override;

  void render(const mat4&) const override;
  using Element::render;
};

}
}

#endif
