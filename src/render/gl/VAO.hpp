#ifndef DIGGLER_RENDER_GL_VAO_HPP
#define DIGGLER_RENDER_GL_VAO_HPP

#include "../../Platform.hpp"

#include <vector>

#include <epoxy/gl.h>

#include "FeatureSupport.hpp"
#include "Util.hpp"
#include "VBO.hpp"

namespace Diggler {
namespace Render {
namespace gl {

class VAO {
protected:
  GLuint m_id;
  GLuint m_eabId;
  struct VertexAttrib {
    GLuint vboId;
    GLuint index;
    GLint size;
    GLenum type;
    GLsizei stride, offset;
    bool normalize;
  };
  std::vector<VertexAttrib> m_vertexAttribs;

public:
  // Ctor / dtor
  VAO() :
    m_id(0),
    m_eabId(0) {
    if (FeatureSupport::VAO) {
      glGenVertexArrays(1, &m_id);
    }
  }

  ~VAO() {
    if (FeatureSupport::VAO) {
      glDeleteVertexArrays(1, &m_id);
    }
  }

  // Copy
  VAO(const VAO&) = delete;
  VAO& operator=(const VAO&) = delete;

  // Move
  VAO(VAO&&) = default;
  VAO& operator=(VAO&&) = default;


  operator GLuint() const {
    return m_id;
  }

  GLuint id() const {
    return m_id;
  }


  void vertexAttrib(const VBO &vbo, GLuint index, GLint size, GLenum type, GLsizei stride,
    GLsizei offset = 0, bool normalize = false) {
    if (FeatureSupport::VAO) {
      BoundBufferSave<GL_VERTEX_ARRAY> save;
      glBindVertexArray(m_id);
      vbo.bind();
      glVertexAttribPointer(index, size, type, normalize, stride,
        reinterpret_cast<GLvoid*>(offset));
    } else {
      m_vertexAttribs.emplace_back();
      VertexAttrib &va = m_vertexAttribs.back();
      va.vboId = vbo.id();
      va.index = index;
      va.size = size;
      va.type = type;
      va.stride = stride;
      va.offset = offset;
      va.normalize = normalize;
    }
  }

  void bindElementArrayBuffer(const VBO &eab) {
    if (FeatureSupport::VAO) {
      BoundBufferSave<GL_VERTEX_ARRAY> save;
      glBindVertexArray(m_id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
    } else {
      m_eabId = eab.id();
    }
  }


  void bind() const {
    if (FeatureSupport::VAO) {
      glBindVertexArray(m_id);
    } else {
      BoundBufferSave<GL_ARRAY_BUFFER> vboSave;
      BoundBufferSave<GL_ELEMENT_ARRAY_BUFFER> eabSave;
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eabId);
      GLuint boundVbo = 0;
      for (const VertexAttrib &va : m_vertexAttribs) {
        if (boundVbo != va.vboId) {
          glBindBuffer(GL_ARRAY_BUFFER, va.vboId);
          boundVbo = va.vboId;
        }
        glVertexAttribPointer(va.index, va.size, va.type, va.normalize, va.stride,
          reinterpret_cast<GLvoid*>(va.offset));
      }
    }
  }

  static void unbind() {
    if (FeatureSupport::VAO) {
      glBindVertexArray(0);
    }
  }
};

}
}
}

#endif /* DIGGLER_RENDER_GL_VAO_HPP */
