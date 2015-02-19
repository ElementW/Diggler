#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <GL/glew.h>
#include <string>

namespace Diggler {
class Texture {
public:
	enum class PixelFormat {
		RGB,
		RGBA,
		Monochrome8
	};
	enum class Filter {
		Nearest,
		Linear
	};
	enum class Wrapping {
		Repeat,
		Clamp
	};
private:
	GLuint id;
	int w, h;
	PixelFormat m_format;
	void create();
	void setPlaceholder(bool makeXor);

public:
	Texture(int w, int h, PixelFormat format = PixelFormat::RGB, bool makeXor = false);
	Texture(int w, int h, uint8_t *data, PixelFormat format = PixelFormat::RGB);
	Texture(const std::string& path, PixelFormat format = PixelFormat::RGB);
	GLuint getId() const;
	operator GLuint() const { return getId(); }
	void resize(int w, int h);
	int getW();
	int getH();
	PixelFormat getPixelFormat();
	int getRequiredBufferSize();
	void getTexture(uint8_t *data);
	void setTexture(int w, int h, uint8_t *data, PixelFormat format = PixelFormat::RGB);
	void setFiltering(Filter min, Filter mag);
	void setWrapping(Wrapping s, Wrapping t);
	void bind() const;
	void bind(int number) const;
	~Texture();
};
}

#endif