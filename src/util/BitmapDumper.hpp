#ifndef DIGGLER_BITMAP_DUMPER_HPP
#define DIGGLER_BITMAP_DUMPER_HPP

#include <string>

namespace Diggler {

class BitmapDumper final {
	BitmapDumper() = delete;

public:
	static bool dumpAsPpm(int w, int h, const void *data, const char *path);
	static inline bool dumpAsPpm(int w, int h, const void *data, const std::string &path) {
		return dumpAsPpm(w, h, data, path.c_str());
	}
};

}

#endif
