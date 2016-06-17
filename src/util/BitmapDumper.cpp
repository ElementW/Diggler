#include "BitmapDumper.hpp"
#include <cstdio>

namespace Diggler {

bool BitmapDumper::dumpAsPpm(int w, int h, const void *data, const char *path) {
	const char *bytes = (const char*) data;
	FILE *fpr = fopen(path, "wb");
	if (fpr == nullptr) {
		return false;
	}
	fprintf(fpr, "P6\n%d %d\n255\n", w, h);
	for (int n = 0; n < w * h; n += 4) {
		fwrite((const void*)&bytes[n], 1, 3, fpr);
	}
	fclose(fpr);
	return true;
}

}
