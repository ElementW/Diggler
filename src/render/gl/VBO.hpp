#ifndef DIGGLER_RENDER_GL_VBO_HPP
#define DIGGLER_RENDER_GL_VBO_HPP

#include "../../Platform.hpp"

#include <vector>

#include <epoxy/gl.h>

#include "Util.hpp"

namespace Diggler {
namespace Render {
namespace gl {

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
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(T), data.data(), usage);
  }
  template <typename T> void setData(const T *data, uint count, GLenum usage = GL_STATIC_DRAW) {
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(T), data, usage);
  }
  template <typename T> void setSubData(const T *data, uint offset, uint count) {
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(T), count*sizeof(T), data);
  }
  void bind() const;
  int getSize() const;
};

}
}
}

#endif /* DIGGLER_RENDER_GL_VBO_HPP */
