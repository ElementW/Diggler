#include "FontManager.hpp"

namespace diggler {
namespace ui {

FontManager::FontManager(Game &G) : G(G) {
}

void FontManager::loadFont(const std::string &path, const std::string &name) {
  m_fonts.emplace(std::piecewise_construct,
    std::forward_as_tuple(name),
    std::forward_as_tuple(std::make_shared<Font>(&G, path)));
}

std::shared_ptr<Font> FontManager::getFont(const std::string &name) {
  if (name.size() == 0) {
    return m_defaultFont;
  }
  return m_fonts.at(name);
}

void FontManager::setDefaultFont(const std::string &name) {
  m_defaultFontName = name;
  m_defaultFont = m_fonts.at(name);
}

std::shared_ptr<Font> FontManager::getDefaultFont() const {
  return m_defaultFont;
}

}
}
