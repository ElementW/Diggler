#ifndef DIGGLER_RENDER_GL_VBO_HPP
#define DIGGLER_RENDER_GL_VBO_HPP

#include <vector>

#include "OpenGL.hpp"

#include "../../platform/PreprocUtils.hpp"

namespace Diggler {
namespace Render {
namespace gl {

class VBO {
protected:
  GLuint m_id, m_size;
  GLenum m_usage;

  void setSubDataInternal(const void *data, uint offset, uint count);
  void setDataInternal(const void *data, uint count, GLenum usage);

public:
  // Ctor / dtor
  VBO();
  ~VBO();

  nocopymove(VBO);


  operator GLuint() const {
    return m_id;
  }

  GLuint id() const {
    return m_id;
  }


  void resize(uint size, GLenum usage = GL_STATIC_DRAW);

  void resizeGrow(uint size, GLenum usage = GL_STATIC_DRAW) {
    if (size > m_size || m_usage != usage) {
      resize(size, usage);
    }
  }

  GLuint size() const {
    return m_size;
  }


  GLenum usage() {
    return m_usage;
  }


  template <typename T> void setData(const T *data, uint count, GLenum usage = GL_STATIC_DRAW) {
    setDataInternal(data, count*sizeof(T), usage);
  }

  template <typename T> void setData(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
     setDataInternal(data.data(), data.size()*sizeof(T), usage);
  }


  template <typename T> void setDataGrow(const T *data, uint count, GLenum usage = GL_STATIC_DRAW) {
    const GLuint targetSize = count*sizeof(T);
    if (targetSize <= m_size && m_usage == usage) {
      setSubData(data, 0, count);
    } else {
      setData(data, count, usage);
    }
  }

  template <typename T> void setDataGrow(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
    setDataGrow(data.data(), data.size(), usage);
  }


  template <typename T> void setSubData(const T *data, uint offset, uint count) {
    setSubDataInternal(data, offset*sizeof(T), count*sizeof(T));
  }

  template <typename T> void setSubData(const std::vector<T>& data, uint offset) {
    setSubDataInternal(data.data(), offset*sizeof(T), data.size()*sizeof(T));
  }


  void bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
  }

  static void unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
};

}
}
}

#endif /* DIGGLER_RENDER_GL_VBO_HPP */
