#include "StringUtil.hpp"

#include <sstream>
#include <utility>

namespace diggler {
namespace Util {

std::vector<std::string> explode(const std::string &s, char delim) {
  std::vector<std::string> result;
  std::istringstream iss(s);
  for (std::string token; std::getline(iss, token, delim); ) {
      result.push_back(std::move(token));
  }
  return result;
}

std::vector<std::u32string> explode(const std::u32string &s, char32_t delim) {
  std::vector<std::u32string> result;
  std::basic_istringstream<char32_t> iss(s);
  for (std::u32string token; std::getline(iss, token, delim); ) {
      result.push_back(std::move(token));
  }
  return result;
}

}
}
