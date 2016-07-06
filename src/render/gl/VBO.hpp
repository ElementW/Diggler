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
protected:
  GLuint m_id, m_size;
  GLenum m_usage;

public:
  // Ctor / dtor
  VBO() :
    m_id(0),
    m_size(0),
    m_usage(GL_STATIC_DRAW) {
    glGenBuffers(1, &m_id);
  }

  ~VBO() {
    glDeleteBuffers(1, &m_id);
  }

  // Copy
  VBO(const VBO&) = delete;
  VBO& operator=(const VBO&) = delete;

  // Move
  VBO(VBO&&) = default;
  VBO& operator=(VBO&&) = default;


  operator GLuint() const {
    return m_id;
  }

  GLuint id() const {
    return m_id;
  }


  void resize(uint size, GLenum usage = GL_STATIC_DRAW) {
    BoundBufferSave<GL_ARRAY_BUFFER> save;

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    m_size = size;
    glBufferData(GL_ARRAY_BUFFER, m_size, nullptr, usage);
  }

  GLuint size() const {
    return m_size;
  }


  GLenum usage() {
    return m_usage;
  }


  template <typename T> void setData(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    m_size = data.size()*sizeof(T);
    m_usage = usage;
    glBufferData(GL_ARRAY_BUFFER, m_size, data.data(), m_usage);
  }

  template <typename T> void setData(const T *data, uint count, GLenum usage = GL_STATIC_DRAW) {
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    m_size = count*sizeof(T);
    m_usage = usage;
    glBufferData(GL_ARRAY_BUFFER, m_size, data, m_usage);
  }


  template <typename T> void setDataKeepSize(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    const GLuint targetSize = data.size()*sizeof(T);
    if (targetSize <= m_size && m_usage == usage) {
      glBufferSubData(GL_ARRAY_BUFFER, 0, targetSize, data.data());
    } else {
      m_size = targetSize;
      m_usage = usage;
      glBufferData(GL_ARRAY_BUFFER, m_size, data.data(), m_usage);
    }
  }

  template <typename T> void setDataKeepSize(const T *data, uint count, GLenum usage = GL_STATIC_DRAW) {
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    const GLuint targetSize = count*sizeof(T);
    if (targetSize <= m_size && m_usage == usage) {
      glBufferSubData(GL_ARRAY_BUFFER, 0, targetSize, data);
    } else {
      m_size = targetSize;
      m_usage = usage;
      glBufferData(GL_ARRAY_BUFFER, m_size, data, m_usage);
    }
  }


  template <typename T> void setSubData(const std::vector<T>& data, uint offset) {
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(T), data.size()*sizeof(T), data.data());
  }

  template <typename T> void setSubData(const T *data, uint offset, uint count) {
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(T), count*sizeof(T), data);
  }


  void bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
  }
};

}
}
}

#endif /* DIGGLER_RENDER_GL_VBO_HPP */
