#include "TexturePacker.hpp"

#include <cmath>
#include <cstring>
#include <stdexcept>
#include <thread>

#include <stb_image.h>

#include "../Game.hpp"
#include "../render/Renderer.hpp"
#include "../Texture.hpp"
#include "BitmapDumper.hpp"
#include "Log.hpp"

using namespace std;

#define ENABLE_TIMING 0
#if ENABLE_TIMING
  #include <chrono>
#endif

namespace Diggler {
namespace Util {

using namespace Logging::LogLevels;

static const char *TAG = "TexturePacker";

// https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72
static void HSVtoRGB(float h, float s, float v, float &r, float &g, float &b) {
  const float c = v * s; // Chroma
  const float prime = fmod(h / 60.f, 6.f);
  const float x = c * (1 - fabs(fmod(prime, 2.f) - 1));
  const float m = v - c;

  if (0 <= prime && prime < 1) {
    r = c;
    g = x;
    b = 0;
  } else if (1 <= prime && prime < 2) {
    r = x;
    g = c;
    b = 0;
  } else if (2 <= prime && prime < 3) {
    r = 0;
    g = c;
    b = x;
  } else if (3 <= prime && prime < 4) {
    r = 0;
    g = x;
    b = c;
  } else if (4 <= prime && prime < 5) {
    r = x;
    g = 0;
    b = c;
  } else if (5 <= prime && prime < 6) {
    r = c;
    g = 0;
    b = x;
  } else {
    r = 0;
    g = 0;
    b = 0;
  }

  r += m;
  g += m;
  b += m;
}

TexturePacker::TexturePacker(Game &G, uint w, uint h) :
  atlasWidth(w), atlasHeight(h), m_freezeTexUpdate(false) {
  if (w <= 2 || h <= 2)
    throw std::invalid_argument("Bad dimensions");

  atlasData = std::make_unique<uint8[]>(w * h * 4);
  memset(atlasData.get(), 0, w * h * 4);
  atlasTex = G.R->textureManager->createTexture(w, h, PixelFormat::RGBA);
  updateTex();
}

TexturePacker::~TexturePacker() {
}

TexturePacker::Coord TexturePacker::add(const std::string& path) {
  // Load image
  int width, height, channels;
  unsigned char *ptr = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  if (!ptr || !width || !height) {
    Log(Error, TAG) << "Could not load '" << path << "': " << stbi_failure_reason();
    if (!m_defaultTexture) {
      m_defaultTexture.reset(new uint8[8*8*4]);
      uint i = 0;
      for (uint8 y = 0; y < 8; ++y) {
        for (uint8 x = 0; x < 8; ++x) {
          m_defaultTexture[i] = m_defaultTexture[i + 1] = m_defaultTexture[i + 2] = (x ^ y) * 32;
          m_defaultTexture[i + 3] = 255;
          i += 4;
        }
      }
      int x = 0, y = 0, xi = 1, yi = 0; uint c = 0;
      while (true) {
        float r, g, b;
        i = (y * 8 + x) * 4;
        HSVtoRGB(c * (360.f / (8+8+6+6)), 1.f, 1.f, r, g, b);
        m_defaultTexture[i    ] = static_cast<uint8>(r * 255);
        m_defaultTexture[i + 1] = static_cast<uint8>(g * 255);
        m_defaultTexture[i + 2] = static_cast<uint8>(b * 255);
        m_defaultTexture[i + 3] = 255;
        if (x == 7) {
          if (y == 0) {
            xi = 0; yi = 1;
          } else if (y == 7) {
            xi = -1; yi = 0;
          }
        } else if (x == 0) {
          if (y == 7) {
            xi = 0; yi = -1;
          } else if (y == 1) {
            break;
          }
        }
        x += xi; y += yi; ++c;
      }
    }
    return add(8, 8, 4, m_defaultTexture.get());
  }
  if (width % 4 != 0 || height % 4 != 0) {
    Log(Error, TAG) << path << " is bad: " << width << 'x' << height;
    stbi_image_free(ptr);
    return Coord { 0, 0, 0, 0 };
  }

  Coord result = add(width, height, channels, ptr);

  // Free the image buffer
  stbi_image_free(ptr);
  return result;
}

using Coord = TexturePacker::Coord;
static bool coordCollides(const Coord &c, const std::vector<Coord> &cs) {
  for (const Coord &tc : cs) {
    bool xOverlap = (c.x >= tc.x && c.x < tc.u) || (c.u > tc.x && c.u <= tc.u);
    bool yOverlap = (c.y >= tc.y && c.y < tc.v) || (c.v > tc.y && c.v <= tc.v);
    /*getDebugStream() << "Collide? {" << c.x << ", " << c.y << ", " << c.u << ", " << c.v
      << "} {" << tc.x << ", " << tc.y << ", " << tc.u << ", " << tc.v << "} = "
      << xOverlap << '&' << yOverlap << std::endl;*/
    if (xOverlap && yOverlap)
      return true;
  }
  return false;
}

static Coord findCoordinates(const std::vector<Coord> &cs, int w, int h, int mx, int my) {
  // Almost-naïve find algorithm
  // Actually not *that* dumb
  Coord c {0, 0, w, h};
  if (!coordCollides(c, cs))
    return c;
  for (const Coord &tc : cs) {
    // Right
    c.x = tc.u; c.y = tc.y;
    c.u = c.x + w; c.v = c.y + h;
    if (c.u < mx && c.v < my && !coordCollides(c, cs))
      return c;
    // Bottom
    c.x = tc.x; c.y = tc.v;
    c.u = c.x + w; c.v = c.y + h;
    if (c.u < mx && c.v < my && !coordCollides(c, cs))
      return c;
  }
  throw std::runtime_error("No more space found on atlas");
}

TexturePacker::Coord TexturePacker::add(int width, int height, int channels, const uint8 *data) {
  // Find a good coord
  Coord c = findCoordinates(coords, width, height, atlasWidth, atlasHeight);
  coords.push_back(c);
  uint16 targetX = c.x, targetY = c.y;

#if ENABLE_TIMING
  auto t1 = std::chrono::high_resolution_clock::now();
#endif
  // Blit the texture onto the atlas
  if (channels == 4) {
    for(int sourceY = 0; sourceY < height; ++sourceY) {
      int fromPad = sourceY * width;
      int toPad = (targetY + sourceY) * atlasWidth;
      memcpy(&atlasData[(toPad+targetX)*4], &data[fromPad*4], width*4);
    }
  } else if (channels == 3) {
    for(int sourceY = 0; sourceY < height; ++sourceY) {
      int fromPad = sourceY * width;
      int toPad = (targetY + sourceY) * atlasWidth;
      for(int sourceX = 0; sourceX < width; ++sourceX) {
        atlasData[(toPad+targetX+sourceX)*4 + 0] = data[(fromPad+sourceX)*3 + 0];
        atlasData[(toPad+targetX+sourceX)*4 + 1] = data[(fromPad+sourceX)*3 + 1];
        atlasData[(toPad+targetX+sourceX)*4 + 2] = data[(fromPad+sourceX)*3 + 2];
        atlasData[(toPad+targetX+sourceX)*4 + 3] = 255;
      }
    }
  }
#if ENABLE_TIMING
  auto t2 = std::chrono::high_resolution_clock::now();
  getDebugStream() << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() << std::endl;
#endif

  uint glScaleX = (1 << (sizeof(Coord::x)*8))/atlasWidth,
       glScaleY = (1 << (sizeof(Coord::y)*8))/atlasHeight;

  updateTex();

  return Coord {
    targetX*glScaleX,
    targetY*glScaleY,
    (targetX + width)*glScaleX,
    (targetY + height)*glScaleY,
  };
}

void TexturePacker::updateTex() {
  if (m_freezeTexUpdate)
    return;
  auto atlasCopy = std::make_unique<uint8[]>(atlasWidth * atlasHeight * 4);
  std::memcpy(atlasCopy.get(), atlasData.get(), atlasWidth * atlasHeight * 4);
  atlasTex->setTexture(std::move(atlasCopy), PixelFormat::RGBA);
  // BitmapDumper::dumpAsPpm(atlasWidth, atlasHeight, atlasData, "/tmp/diggler_atlas.ppm");
}

void TexturePacker::freezeTexUpdate(bool f) {
  m_freezeTexUpdate = f;
  if (!f)
    updateTex();
}

}
}
