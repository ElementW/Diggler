#include "Encoding.hpp"

#include <codecvt>
#include <locale>

namespace Diggler {
namespace Util {
namespace Encoding {

std::string toUtf8(const std::u32string &str) {
  return std::wstring_convert< std::codecvt_utf8<char32_t>, char32_t >{}.to_bytes(str);
}

std::u32string toUtf32(const std::string &str) {
  return std::wstring_convert< std::codecvt_utf8<char32_t>, char32_t >{}.from_bytes(str);
}

}
}
}
