#ifndef DIGGLER_UTIL_ENCODING_HPP
#define DIGGLER_UTIL_ENCODING_HPP

#include <string>

namespace diggler {
namespace Util {
namespace Encoding {

std::string toUtf8(const std::u32string&);
std::u32string toUtf32(const std::string&);

}
}
}

#endif /* DIGGLER_UTIL_ENCODING_HPP */
