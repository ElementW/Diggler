#include "FBO.hpp"
#include "GlUtils.hpp"
#ifdef IN_IDE_PARSER
	void glCheck();
#else
	#define glCheck() { GLenum glErr = glGetError(); \
		if (glErr) { \
			getDebugStream() << GlUtils::getErrorString(glErr) << std::endl; \
		} glErr = glCheckFramebufferStatus(GL_FRAMEBUFFER); if (glErr != GL_FRAMEBUFFER_COMPLETE) { \
			getDebugStream() << GlUtils::getErrorString(glErr) << std::endl; \
		} \
	}
#endif

namespace Diggler {

FBO::FBO(int w, int h, Texture::PixelFormat format, bool stencil) : m_hasStencil(stencil) {
	GLint currentBoundFBO; glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBoundFBO);
	GLint currentBoundRBO; glGetIntegerv(GL_RENDERBUFFER_BINDING, &currentBoundRBO);

	glGetError(); // Flush previous errors

	tex = new Texture(w, h, format);
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex, 0);
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);

	glGenRenderbuffers(1, &rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	if (stencil)
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);

	glCheck();

	glBindFramebuffer(GL_FRAMEBUFFER, currentBoundFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, currentBoundRBO);
}

void FBO::resize(int w, int h) {
	GLint currentBoundRBO; glGetIntegerv(GL_RENDERBUFFER_BINDING, &currentBoundRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	if (m_hasStencil)
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, currentBoundRBO);

	tex->resize(w, h);
}

bool FBO::hasStencil() const {
	return m_hasStencil;
}

void FBO::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FBO::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::~FBO() {
	glDeleteFramebuffers(1, &id);
	delete tex;
	glDeleteRenderbuffers(1, &rboId);
}

}
