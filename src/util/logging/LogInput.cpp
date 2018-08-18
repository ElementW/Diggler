#include "LogInput.hpp"

#include <iomanip>
#include <sstream>
#include <string>

#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec4.hpp>

#include "../Encoding.hpp"

namespace diggler {
namespace Util {
namespace Logging {

LogInput::LogInput(Logger &sink, LogLevel lvl, const std::string &tag) :
  sink(sink), lvl(lvl), tag(tag) {
}

LogInput::~LogInput() {
  if (not buf.empty()) {
    sink.log(buf, lvl, tag);
  }
}

LogInput::LogInput(LogInput &&o) :
  sink(o.sink), lvl(o.lvl), buf(o.buf) {
  o.buf.clear();
}

LogInput& LogInput::operator<<(const char *s) {
  buf.append(s);
  return *this;
}

LogInput& LogInput::operator<<(const char32_t *s) {
  buf.append(Encoding::toUtf8(s));
  return *this;
}

LogInput& LogInput::operator<<(const std::string &s) {
  buf.append(s);
  return *this;
}

LogInput& LogInput::operator<<(const std::u32string &s) {
  buf.append(Encoding::toUtf8(s));
  return *this;
}


LogInput& LogInput::operator<<(bool b) {
  buf.append(b ? "true" : "false");
  return *this;
}

LogInput& LogInput::operator<<(char c) {
  buf.push_back(c);
  return *this;
}

LogInput& LogInput::operator<<(uint8_t i) {
  buf.append(std::to_string(i));
  return *this;
}

LogInput& LogInput::operator<<(int8_t i) {
  buf.append(std::to_string(i));
  return *this;
}

LogInput& LogInput::operator<<(uint16_t i) {
  buf.append(std::to_string(i));
  return *this;
}

LogInput& LogInput::operator<<(int16_t i) {
  buf.append(std::to_string(i));
  return *this;
}

LogInput& LogInput::operator<<(uint32_t i) {
  buf.append(std::to_string(i));
  return *this;
}

LogInput& LogInput::operator<<(int32_t i) {
  buf.append(std::to_string(i));
  return *this;
}

LogInput& LogInput::operator<<(uint64_t i) {
  buf.append(std::to_string(i));
  return *this;
}

LogInput& LogInput::operator<<(int64_t i) {
  buf.append(std::to_string(i));
  return *this;
}

LogInput& LogInput::operator<<(float f) {
  buf.append(std::to_string(f));
  return *this;
}

LogInput& LogInput::operator<<(double f) {
  buf.append(std::to_string(f));
  return *this;
}

#ifdef __APPLE__
LogInput& LogInput::operator<<(size_t i) {
  buf.append(std::to_string(i));
  return *this;
}
#endif

LogInput& LogInput::operator<<(const void *ptr) {
  std::stringbuf hbuf;
  std::ostream os(&hbuf);
  os << "0x" << std::setfill('0') << std::setw(sizeof(ptr) * 2) << std::hex << (uint64_t)ptr;
  buf.append(hbuf.str());
  return *this;
}


LogInput& LogInput::operator<<(const glm::vec2 &v) {
  *this << '(' << v.x << ", " << v.y << ')';
  return *this;
}

LogInput& LogInput::operator<<(const glm::vec3 &v) {
  *this << '(' << v.x << ", " << v.y << ", " << v.z << ')';
  return *this;
}

LogInput& LogInput::operator<<(const glm::vec4 &v) {
  *this << '(' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')';
  return *this;
}

}
}
}
