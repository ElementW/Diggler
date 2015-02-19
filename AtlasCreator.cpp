#include "AtlasCreator.hpp"
#include "stb_image.h"
//#include <chrono>
#include <cmath>
#include <stdexcept>

namespace Diggler {

AtlasCreator::AtlasCreator(int w, int h, int uw, int uh) : atlasWidth(w), atlasHeight(h), unitWidth(uw), unitHeight(uh), lastX(0), lastY(0) {
	if (w < 1 || h < 1 || uw < 1 || uh < 1 || uw > w || uh > h)
		throw std::invalid_argument("Bad dimensions");
	if (fmod((1 << (sizeof(Coord::x)*8))/(float)atlasWidth, 1) != 0 ||
		fmod((1 << (sizeof(Coord::y)*8))/(float)atlasHeight, 1) != 0)
		throw std::invalid_argument("Atlas W/H is not divisor of Coord's type");
	
	atlasData = new uint8[w * h * 4];
	std::fill_n(atlasData, w * h * 4, static_cast<uint8>(0));
}

AtlasCreator::Coord AtlasCreator::add(const std::string& path) {
	// Load image
	int width, height, channels;
	unsigned char *ptr = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!ptr || !width || !height) {
		const char *ptr =
			"\377\0\0\377\377\0\0\377\377\0\0\377\377\0\0\377\377\0\0\377\377\0\0\377"
			"\377\0\0\377\377\0\0\377\377\0\0\377\210\210\210\377\210\210\210\377\36\377"
			"\0\377\36\377\0\377\210\210\210\377\210\210\210\377\377\0\0\377\377\0\0\377"
			"\210\210\210\377\36\377\0\377\210\210\210\377\210\210\210\377\36\377\0\377"
			"\210\210\210\377\377\0\0\377\377\0\0\377\210\210\210\377\210\210\210\377"
			"\210\210\210\377\36\377\0\377\210\210\210\377\210\210\210\377\377\0\0\377"
			"\377\0\0\377\210\210\210\377\210\210\210\377\36\377\0\377\210\210\210\377"
			"\210\210\210\377\210\210\210\377\377\0\0\377\377\0\0\377\210\210\210\377"
			"\210\210\210\377\210\210\210\377\210\210\210\377\210\210\210\377\210\210"
			"\210\377\377\0\0\377\377\0\0\377\210\210\210\377\210\210\210\377\36\377\0"
			"\377\210\210\210\377\210\210\210\377\210\210\210\377\377\0\0\377\377\0\0"
			"\377\377\0\0\377\377\0\0\377\377\0\0\377\377\0\0\377\377\0\0\377\377\0\0"
			"\377\377\0\0\377"; // Made in 3 seconds under GIMP
		return add(8, 8, 4, (const uint8*)ptr);
	}
	if (width > unitWidth || height > unitHeight || width % 4 != 0 || height % 4 != 0) {
		getDebugStream() << path << " is bad: " << width << 'x' << height << std::endl;
		stbi_image_free(ptr);
		return Coord { 0, 0, 0, 0 };
	}
	
	Coord result = add(width, height, channels, ptr);
	
	// Free the image buffer
	stbi_image_free(ptr);
	return result;
}

AtlasCreator::Coord AtlasCreator::add(int width, int height, int channels, const uint8 *data) {
	// Find a good coord
	uint16 targetX, targetY;
	if (lastX + unitWidth > atlasWidth) {
		targetX = 0;
		targetY = lastY + unitHeight;
	} else {
		targetX = lastX;
		targetY = lastY;
	}
	
	//auto t1 = std::chrono::high_resolution_clock::now();
	// Blit the texture onto the atlas
	for(int sourceY = 0; sourceY < height; ++sourceY) {
		int fromPad = sourceY * width;
		int toPad = (targetY + sourceY) * atlasWidth;
		for(int sourceX = 0; sourceX < width; sourceX += 2) {
			int from = (fromPad + sourceX) * 4;
			int to = (toPad + (targetX + sourceX)) * 4;
			
			// MMX-like copy, fast (actual MMX/SSE would be f'kin fast)
#if HAS_NATIVE_64BIT
			*((int64*)(&atlasData[to])) = *((int64*)(&data[from]));
#else
			*((int32*)(&atlasData[to])) = *((int32*)(&data[from]));
			*((int32*)(&atlasData[to + 4])) = *((int32*)(&data[from + 4]));
#endif
		}
	}
	//auto t2 = std::chrono::high_resolution_clock::now();
	//getDebugStream() << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() << std::endl;
	
	lastX = targetX + unitWidth;
	lastY = targetY;
	uint glScaleX = (1 << (sizeof(Coord::x)*8))/atlasWidth,
		 glScaleY = (1 << (sizeof(Coord::y)*8))/atlasHeight;
	//getDebugStream() << width << 'x' << height << '@' << targetX << ',' << targetY << " * " << glScaleX << ',' << glScaleY << std::endl;
	
	Coord c = {
		targetX*glScaleX,
		targetY*glScaleY,
		(targetX + width)*glScaleX-1,
		(targetY + height)*glScaleY-1,
	};
	//getDebugStream() << '{' << c.x << ',' << c.y << ';' << c.u << ',' << c.v << '}' << std::endl;
	//getDebugStream() << '{' << targetX << ',' << targetY << "→" << (targetX + width) << ',' << (targetY + height) << '}' << std::endl;
	return c;
}

Texture* AtlasCreator::getAtlas() {
	return new Texture(atlasWidth, atlasHeight, atlasData, Texture::PixelFormat::RGBA);
}

AtlasCreator::~AtlasCreator() {
	delete[] atlasData;
}


}