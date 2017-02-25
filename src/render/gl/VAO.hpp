#ifndef DIGGLER_RENDER_GL_VAO_HPP
#define DIGGLER_RENDER_GL_VAO_HPP

#include "../../Platform.hpp"

#include <vector>

#include "OpenGL.hpp"

#include "FeatureSupport.hpp"
#include "Util.hpp"
#include "VBO.hpp"

namespace Diggler {
namespace Render {
namespace gl {

class VAO {
protected:
  friend struct Config;

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

  struct Config {
    VAO &vao;
    GLuint prevVAO;

    Config(VAO &vao) :
      vao(vao) {
      if (FeatureSupport::VAO) {
        getUInteger(GL_VERTEX_ARRAY_BINDING, prevVAO);
        glBindVertexArray(vao.m_id);
      }
    }

    Config(const Config&) = delete;

    Config(Config &&cfg) :
      vao(cfg.vao),
      prevVAO(cfg.prevVAO) {
    }

    void commit() {
      if (FeatureSupport::VAO) {
        glBindVertexArray(prevVAO);
      }
    }

    void vertexAttrib(const VBO &vbo, GLuint index, GLint size, GLenum type, GLsizei stride,
      GLsizei offset = 0, bool normalize = false) {
      if (FeatureSupport::VAO) {
        BoundBufferSave<GL_ARRAY_BUFFER> vboSave;
        vbo.bind();
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, type, normalize, stride,
          reinterpret_cast<GLvoid*>(offset));
      } else {
        vao.m_vertexAttribs.emplace_back();
        VertexAttrib &va = vao.m_vertexAttribs.back();
        va.vboId = vbo.id();
        va.index = index;
        va.size = size;
        va.type = type;
        va.stride = stride;
        va.offset = offset;
        va.normalize = normalize;
      }
    }

    void elementArrayBuffer(const VBO &eab) {
      if (FeatureSupport::VAO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
      } else {
        vao.m_eabId = eab.id();
      }
    }
  };

  Config configure() {
    return Config(*this);
  }

  void bind() const {
    if (FeatureSupport::VAO) {
      glBindVertexArray(m_id);
    } else {
      BoundBufferSave<GL_ARRAY_BUFFER> vboSave;
      GLuint boundVbo = 0;
      for (const VertexAttrib &va : m_vertexAttribs) {
        if (boundVbo != va.vboId) {
          glBindBuffer(GL_ARRAY_BUFFER, va.vboId);
          boundVbo = va.vboId;
        }
        glEnableVertexAttribArray(va.index);
        glVertexAttribPointer(va.index, va.size, va.type, va.normalize, va.stride,
          reinterpret_cast<GLvoid*>(va.offset));
      }
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eabId);
    }
  }

  void unbind() const {
    if (FeatureSupport::VAO) {
      glBindVertexArray(0);
    } else {
      for (const VertexAttrib &va : m_vertexAttribs) {
        glDisableVertexAttribArray(va.index);
      }
    }
  }
};

}
}
}

#endif /* DIGGLER_RENDER_GL_VAO_HPP */
