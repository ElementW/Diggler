#ifndef DIGGLER_RENDER_GL_VBO_HPP
#define DIGGLER_RENDER_GL_VBO_HPP

#include <vector>

#include <epoxy/gl.h>

#include "../../platform/PreprocUtils.hpp"
#include "FeatureSupport.hpp"
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
    if (FeatureSupport::DSA_ARB) {
      glCreateBuffers(1, &m_id);
    } else {
      glGenBuffers(1, &m_id);
    }
  }

  ~VBO() {
    glDeleteBuffers(1, &m_id);
  }

  nocopymove(VBO);


  operator GLuint() const {
    return m_id;
  }

  GLuint id() const {
    return m_id;
  }


  void resize(uint size, GLenum usage = GL_STATIC_DRAW) {
    if (size == 0) {
      return;
    }
    m_size = size;
    m_usage = usage;
    if (FeatureSupport::DSA_ARB) {
      glNamedBufferData(m_id, m_size, nullptr, m_usage);
    } else if (FeatureSupport::DSA_EXT) {
      glNamedBufferDataEXT(m_id, m_size, nullptr, m_usage);
    } else {
      BoundBufferSave<GL_ARRAY_BUFFER> save;
      glBindBuffer(GL_ARRAY_BUFFER, m_id);
      glBufferData(GL_ARRAY_BUFFER, m_size, nullptr, m_usage);
    }
  }

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
    if (count == 0) {
      return;
    }
    m_size = count*sizeof(T);
    m_usage = usage;
    if (FeatureSupport::DSA_ARB) {
      glNamedBufferData(m_id, m_size, data, m_usage);
    } else if (FeatureSupport::DSA_EXT) {
      glNamedBufferDataEXT(m_id, m_size, data, m_usage);
    } else {
      BoundBufferSave<GL_ARRAY_BUFFER> save;
      glBindBuffer(GL_ARRAY_BUFFER, m_id);
      glBufferData(GL_ARRAY_BUFFER, m_size, data, m_usage);
    }
  }

  template <typename T> void setData(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
    setData(data.data(), data.size(), usage);
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
    if (count == 0) {
      return;
    }
    if (FeatureSupport::DSA_ARB) {
      glNamedBufferSubData(m_id, offset*sizeof(T), count*sizeof(T), data);
    } else if (FeatureSupport::DSA_EXT) {
      glNamedBufferSubDataEXT(m_id, offset*sizeof(T), count*sizeof(T), data);
    } else {
      BoundBufferSave<GL_ARRAY_BUFFER> save;
      glBindBuffer(GL_ARRAY_BUFFER, m_id);
      glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(T), count*sizeof(T), data);
    }
  }

  template <typename T> void setSubData(const std::vector<T>& data, uint offset) {
    setSubData(data.data(), data.size(), offset);
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
