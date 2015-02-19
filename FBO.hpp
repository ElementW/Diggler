#ifndef FBO_HPP
#define FBO_HPP
#include <vector>
#include <GL/glew.h>
#include <typeinfo>
#include "Platform.hpp"
#include "Texture.hpp"

namespace Diggler {

class FBO {
private:
	bool m_hasStencil;

public:
	Texture *tex;
	GLuint id, rboId;
	FBO(int w = 640, int h = 480, Texture::PixelFormat format = Texture::PixelFormat::RGB, bool stencil = false);
	operator GLuint() const { return id; }
	void resize(int w, int h);
	void bind();
	void unbind();
	bool hasStencil() const;
	~FBO();
};

}

#endif