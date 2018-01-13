#include "BitmapDumper.hpp"
#include <cstdio>

namespace diggler {

bool BitmapDumper::dumpAsPpm(int w, int h, const void *data, const char *path) {
  const char *bytes = static_cast<const char*>(data);
  FILE *fpr = fopen(path, "wb");
  if (fpr == nullptr) {
    return false;
  }
  fprintf(fpr, "P6\n%d %d\n255\n", w, h);
  for (int n = 0; n < w * h * 4; n += 4) {
    fwrite(static_cast<const void*>(&bytes[n]), 1, 3, fpr);
  }
  fclose(fpr);
  return true;
}

bool BitmapDumper::dumpAsTga(int w, int h, const void *data, PixelFormat pf, const char *path) {
  FILE *fpr = fopen(path, "wb");
  if (fpr == nullptr) {
    return false;
  }
  fwrite("\0\0\2\0\0\0\0\0", 8, 1, fpr);
  fwrite("\0\0\0\0", 4, 1, fpr); // X/Y origin, 0/0
  const uint16 width = w, height = h;
  fwrite(&width, 2, 1, fpr);
  fwrite(&height, 2, 1, fpr);
  const uint8 pixDepth = (pf == PixelFormat::RGB) ? 24 : 32;
  fwrite(&pixDepth, 1, 1, fpr);
  uint8 imgDesc = (pf == PixelFormat::RGB) ? 0 : 8;
  imgDesc |= 1 << 5; // origin: top
  fwrite(&imgDesc, 1, 1, fpr);
  fwrite(data, (pf == PixelFormat::RGB) ? 2 : 3, w * h, fpr);
  fclose(fpr);
  return true;
}

}
