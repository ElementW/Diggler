#ifndef DIGGLER_UTIL_STRING_UTIL_HPP
#define DIGGLER_UTIL_STRING_UTIL_HPP

#include <string>
#include <vector>

namespace diggler {
namespace Util {

class StringUtil {
public:
  StringUtil() = delete;

  static std::vector<std::string> explode(const std::string &s, char delim);
  static std::vector<std::u32string> explode(const std::u32string &s, char32_t delim);
};

}
}

#endif /* DIGGLER_UTIL_STRING_UTIL_HPP */
