#ifndef VBO_HPP
#define VBO_HPP
#include <vector>
#include <GL/glew.h>
#include <typeinfo>
#include "Platform.hpp"
#include <cstring>

namespace Diggler {

class VBO {
private:
	int *m_refcount;

public:
	GLuint id;
	
	// Ctor / dtor
	VBO();
	~VBO();
	// Copy
	VBO(const VBO&);
	VBO& operator=(const VBO&);
	// Move
	VBO(VBO&&);
	VBO& operator=(VBO&&);
	
	operator GLuint() const { return id; }
	template <typename T> void setData(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
		GLint currentBoundArray; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentBoundArray);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(T), data.data(), usage);
		glBindBuffer(GL_ARRAY_BUFFER, currentBoundArray);
	}
	template <typename T> void setData(const T *data, uint count, GLenum usage = GL_STATIC_DRAW) {
		GLint currentBoundArray; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentBoundArray);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, count*sizeof(*data), data, usage);
		glBindBuffer(GL_ARRAY_BUFFER, currentBoundArray);
	}
	void bind() const;
	int getSize() const;
};

}

#endif