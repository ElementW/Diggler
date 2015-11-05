#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP
#include <map>
#include <memory>
#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include "../Platform.hpp"
#include "../VBO.hpp"
#include "Font.hpp"

namespace Diggler {
namespace UI {

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

#endif