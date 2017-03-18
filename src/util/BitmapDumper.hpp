#ifndef DIGGLER_BITMAP_DUMPER_HPP
#define DIGGLER_BITMAP_DUMPER_HPP

#include <string>

#include "../PixelFormat.hpp"

namespace Diggler {

class BitmapDumper final {
  BitmapDumper() = delete;

public:
  static bool dumpAsPpm(int w, int h, const void *data, const char *path);
  static inline bool dumpAsPpm(int w, int h, const void *data, const std::string &path) {
    return dumpAsPpm(w, h, data, path.c_str());
  }

  static bool dumpAsTga(int w, int h, const void *data, PixelFormat, const char *path);
  static inline bool dumpAsTga(int w, int h, const void *data, PixelFormat,
      const std::string &path) {
    return dumpAsPpm(w, h, data, path.c_str());
  }
};

}

#endif
