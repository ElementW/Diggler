#ifndef VBO_HPP
#define VBO_HPP
#include <vector>
#include <typeinfo>
#include <cstring>
#include <epoxy/gl.h>
#include "Platform.hpp"

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
  void setSize(uint size, GLenum usage = GL_STATIC_DRAW);
  template <typename T> void setData(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
    GLint currentBoundArray; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentBoundArray);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(T), data.data(), usage);
    glBindBuffer(GL_ARRAY_BUFFER, currentBoundArray);
  }
  template <typename T> void setData(const T *data, uint count, GLenum usage = GL_STATIC_DRAW) {
    GLint currentBoundArray; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentBoundArray);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(T), data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, currentBoundArray);
  }
  template <typename T> void setSubData(const T *data, uint offset, uint count) {
    GLint currentBoundArray; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentBoundArray);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(T), count*sizeof(T), data);
    glBindBuffer(GL_ARRAY_BUFFER, currentBoundArray);
  }
  void bind() const;
  int getSize() const;
};

}

#endif