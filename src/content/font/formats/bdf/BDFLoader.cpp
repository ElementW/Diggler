#include <string>

#include "../../../../util/StringUtil.hpp"

namespace Diggler {
namespace Content {
namespace Font {
namespace Formats {
namespace BDF {

class BDFLoader {
public:

};

struct XLFD {
  std::string foundry, familyName, weightName;
  enum class Slant : uint8_t {
    Roman,
    Italic,
    Oblique,
    ReverseItalic,
    ReverseOblique,
    Other,
    Polymorphic
  } slant;
  std::string setwidthName, addStyleName;
  int pixelSize, resolutionX, resolutionY;
  enum class Spacing : uint8_t {
    Proportional,
    Monospaced,
    CharCell
  } spacing;
  int averageWidth;
  std::string encoding;

  static XLFD parse(const std::string &line) {
    auto parts = Util::StringUtil::explode(line, '-');
  }
};

}
}
}
}
}
