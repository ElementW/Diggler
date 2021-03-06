#ifndef DIGGLER_UI_FONT_MANAGER_HPP
#define DIGGLER_UI_FONT_MANAGER_HPP

#include "../Platform.hpp"

#include <map>
#include <memory>

#include "Font.hpp"

namespace diggler {
namespace ui {

class FontManager {
private:
  Game &G;

  std::map<std::string, std::shared_ptr<Font>> m_fonts;
  std::string m_defaultFontName;
  std::shared_ptr<Font> m_defaultFont;

public:
  FontManager(Game &G);

  void loadFont(const std::string &path, const std::string &name);
  /**
   * @brief Gets a font using its name.
   * If `name` is an empty string, returns the default font
   * (effectively `getDefaultFont's return)
   */
  std::shared_ptr<Font> getFont(const std::string &name);

  void setDefaultFont(const std::string &name);
  std::shared_ptr<Font> getDefaultFont() const;
};

}
}

#endif /* DIGGLER_UI_FONT_MANAGER_HPP */
