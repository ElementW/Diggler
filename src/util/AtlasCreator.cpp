#include "AtlasCreator.hpp"
#include <stb_image.h>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include <thread>
#include "../Texture.hpp"

#include <cstdio>
using namespace std;

#define ENABLE_TIMING 0
#if ENABLE_TIMING
	#include <chrono>
#endif

namespace Diggler {

AtlasCreator::AtlasCreator(uint w, uint h) :
	atlasWidth(w), atlasHeight(h), m_freezeTexUpdate(false) {
	if (w <= 2 || h <= 2)
		throw std::invalid_argument("Bad dimensions");

	atlasData = new uint8[w * h * 4];
	memset(atlasData, 0, w * h * 4);
	atlasTex = new Texture(w, h, Texture::PixelFormat::RGBA);
	updateTex();
}

AtlasCreator::~AtlasCreator() {
	delete atlasTex;
	delete[] atlasData;
}

AtlasCreator::Coord AtlasCreator::add(const std::string& path) {
	// Load image
	int width, height, channels;
	unsigned char *ptr = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!ptr || !width || !height) {
		return add(2, 2, 4, (const uint8*)"\xFF\x00\x00\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\xFF\x00\x00\xFF");
	}
	if (width % 4 != 0 || height % 4 != 0) {
		getDebugStream() << path << " is bad: " << width << 'x' << height << std::endl;
		stbi_image_free(ptr);
		return Coord { 0, 0, 0, 0 };
	}

	Coord result = add(width, height, channels, ptr);

	// Free the image buffer
	stbi_image_free(ptr);
	return result;
}

using Coord = AtlasCreator::Coord;
static bool coordCollides(const Coord &c, const std::vector<Coord> &cs) {
	for (const Coord &tc : cs) {
		bool xOverlap = (c.x >= tc.x && c.x <= tc.u) || (c.u >= tc.x && c.u <= tc.u);
		bool yOverlap = (c.y >= tc.y && c.y <= tc.v) || (c.v >= tc.y && c.v <= tc.v);
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
	Coord c {0, 0, w-1, h-1};
	if (!coordCollides(c, cs))
		return c;
	for (const Coord &tc : cs) {
		// Right
		c.x = tc.u+1; c.y = tc.y;
		c.u = c.x + w-1; c.v = c.y + h-1;
		if (c.u < mx && c.v < my && !coordCollides(c, cs))
			return c;
		// Bottom
		c.x = tc.x; c.y = tc.v+1;
		c.u = c.x + w-1; c.v = c.y + h-1;
		if (c.u < mx && c.v < my && !coordCollides(c, cs))
			return c;
	}
	throw std::runtime_error("No more space found on atlas");
}

AtlasCreator::Coord AtlasCreator::add(int width, int height, int channels, const uint8 *data) {
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
		(targetX + width)*glScaleX-1,
		(targetY + height)*glScaleY-1,
	};
}

void AtlasCreator::updateTex() {
	if (m_freezeTexUpdate)
		return;
	atlasTex->setTexture(atlasData, Texture::PixelFormat::RGBA);
	// BitmapDumper::dumpAsPpm(atlasWidth, atlasHeight, atlasData, "/tmp/diggler_atlas.ppm");
}

void AtlasCreator::freezeTexUpdate(bool f) {
	m_freezeTexUpdate = f;
	if (!f)
		updateTex();
}

const Texture* AtlasCreator::getAtlas() {
	return atlasTex;
}

}
