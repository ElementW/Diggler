#include "Texture.hpp"
#include "Platform.hpp"
#include "stb_image.h"
#include <cstdio>
#include <cerrno>
#include <cstring>
#define PushBoundTex() GLint currentBoundTex; glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentBoundTex);
#define PopBoundTex() glBindTexture(GL_TEXTURE_2D, currentBoundTex);

#define TEXTURE_LOAD_DEBUG 0

namespace Diggler {

Texture::Texture(int w, int h, Texture::PixelFormat format, bool makeXor) : w(w), h(h), m_format(format) {
	PushBoundTex();
	create();
	setPlaceholder(makeXor);
	PopBoundTex();
}

Texture::Texture(int w, int h, uint8_t* data, Texture::PixelFormat format) {
	PushBoundTex();
	create();
	setTexture(w, h, data, format);
	PopBoundTex();
}

static GLenum getGlTexFormat(Texture::PixelFormat fmt);
Texture::Texture(const std::string& path, PixelFormat format) {
	PushBoundTex();
	create();
	int stbiFormat;
	switch (format) {
	case PixelFormat::RGB:
		stbiFormat = STBI_rgb;
		break;
	case PixelFormat::RGBA:
		stbiFormat = STBI_rgb_alpha;
		break;
	case PixelFormat::Monochrome8:
		stbiFormat = STBI_grey;
		break;
	}
	int width, height, channels;
	unsigned char *ptr = nullptr;
	FILE *fp = fopen(path.c_str(), "rb");
	if (fp != nullptr) {
		ptr = stbi_load_from_file(fp, &width, &height, &channels, stbiFormat);
		w = width; h = height;
	}
	if (ptr && width && height) {
		setTexture(w, h, ptr, format);
		stbi_image_free(ptr);
#if TEXTURE_LOAD_DEBUG
		getDebugStream() << "Loaded image \"" << path << "\" w=" << w << " h=" << h <<
			" c=" << channels << " pf=" << (int)format << " glPf=" << (int)getGlTexFormat(format) << std::endl;
#endif
	} else {
		w = 64; h = 64;
		if (fp == nullptr)
			getErrorStream() << "Failed to open \"" << path << "\" : " << std::strerror(errno) << std::endl;
		else
			getErrorStream() << "Failed to load image \"" << path << "\" : " << stbi_failure_reason() << std::endl;
		setPlaceholder(true);
	}
	PopBoundTex();
}

void Texture::create() {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	setFiltering(Filter::Nearest, Filter::Nearest);
}

GLenum getFilterGlConstant(Texture::Filter filter) {
	switch (filter) {
		case Texture::Filter::Linear:
			return GL_LINEAR;
		case Texture::Filter::Nearest:
			return GL_NEAREST;
		default:
			return 0;
	}
}

void Texture::setFiltering(Filter min, Filter mag) {
	PushBoundTex();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getFilterGlConstant(min));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getFilterGlConstant(mag));
	PopBoundTex();
}

static GLenum getWrapGlConstant(Texture::Wrapping wrap) {
	switch (wrap) {
		case Texture::Wrapping::Clamp:
			return GL_CLAMP;
		case Texture::Wrapping::Repeat:
			return GL_REPEAT;
		default:
			return 0;
	}
}

void Texture::setWrapping(Wrapping s, Wrapping t) {
	PushBoundTex();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrapGlConstant(s));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrapGlConstant(t));
	PopBoundTex();
}

void Texture::setPlaceholder(bool makeXor) {
	// VALVe checkerboard, anyone?
	int pxLength;
	switch (m_format) {
		case PixelFormat::RGB:
			pxLength = 3;
		break;
		case PixelFormat::RGBA:
			pxLength = 4;
		break;
		case PixelFormat::Monochrome8:
			pxLength = 1;
		break;
	}
	unsigned char* white = new unsigned char[w * h * pxLength];
	if (makeXor) {
		for(int x = 0; x < w; x++) // XOR texture
			for(int y = 0; y < h; y++) 
				for(int i = 0; i < pxLength; i++)
						white[i+x*pxLength+y*w*pxLength] = x ^ y;
	} else {
		memset(white, 255, w * h * pxLength);
	}
	setTexture(w, h, white, m_format);
	delete[] white;
}

static GLenum getGlTexFormat(Texture::PixelFormat fmt) {
	switch (fmt) {
		case Texture::PixelFormat::RGB:
			return GL_RGB;
		break;
		case Texture::PixelFormat::RGBA:
			return  GL_RGBA;
		break;
		case Texture::PixelFormat::Monochrome8:
			return GL_R8;
		break;
	}
	return 0;
}

void Texture::setTexture(int w, int h, uint8_t *data, PixelFormat format) {
	this->w = w; this->h = h;
	this->m_format = format;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // This. GL = state machine
	GLenum glFormat = getGlTexFormat(format);
	glTexImage2D(GL_TEXTURE_2D, // target
			0,  // level, 0 = base, no minimap
			glFormat, // internalformat
			w,  // width
			h,  // height
			0,  // border
			glFormat,  // format
			GL_UNSIGNED_BYTE, // type
			data);
}

GLuint Texture::getId() const {
	return id;
}

void Texture::resize(int w, int h) {
	if (this->w == w && this->h == h)
		return;
	PushBoundTex();
	bind();
	GLenum glFormat = getGlTexFormat(m_format);
	glTexImage2D(GL_TEXTURE_2D, // target
		0,  // level, 0 = base, no minimap,
		glFormat, // internalformat
		w,  // width
		h,  // height
		0,  // border
		glFormat,  // format
		GL_UNSIGNED_BYTE, // type
		nullptr);
	this->w = w;
	this->h = h;
	PopBoundTex();
}

int Texture::getW() {
	return w;
}

int Texture::getH() {
	return h;
}

Texture::PixelFormat Texture::getPixelFormat() {
	return m_format;
}

int Texture::getRequiredBufferSize() {
	int texelSize;
	switch (m_format) {
		case PixelFormat::RGB:
			texelSize = 3;
		break;
		case PixelFormat::RGBA:
			texelSize = 4;
		break;
		case PixelFormat::Monochrome8:
			texelSize = 1;
		break;
	}
	return w * h * texelSize;
}

void Texture::getTexture(uint8_t* data) {
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void Texture::bind() const {
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::bind(int number) const {
	glActiveTexture(GL_TEXTURE0+number);
	glBindTexture(GL_TEXTURE_2D, id);
	glActiveTexture(GL_TEXTURE0);
}

Texture::~Texture() {
	glDeleteTextures(1, &id);
}

}