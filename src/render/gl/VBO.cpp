#include "VBO.hpp"

#include "FeatureSupport.hpp"
#include "Util.hpp"

namespace diggler {
namespace render {
namespace gl {

VBO::VBO() :
  m_id(0),
  m_size(0),
  m_usage(GL_STATIC_DRAW) {
  if (FeatureSupport::DSA_ARB) {
    glCreateBuffers(1, &m_id);
  } else {
    glGenBuffers(1, &m_id);
  }
}

VBO::~VBO() {
  glDeleteBuffers(1, &m_id);
}

static inline void checkUsage(GLenum usage) {
#if DEBUG
  switch (usage) {
  case GL_DYNAMIC_COPY:
  case GL_DYNAMIC_DRAW:
  case GL_DYNAMIC_READ:
  case GL_STATIC_COPY:
  case GL_STATIC_DRAW:
  case GL_STATIC_READ:
  case GL_STREAM_COPY:
  case GL_STREAM_DRAW:
  case GL_STREAM_READ:
    return;
  default:
    break;
  }
  throw std::invalid_argument("Invalid VBO usage " + std::to_string(usage));
#endif
}

void VBO::resize(uint size, GLenum usage) {
  checkUsage(usage);
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

void VBO::setDataInternal(const void *data, uint count, GLenum usage) {
  checkUsage(usage);
  m_size = count;
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

void VBO::setSubDataInternal(const void *data, uint offset, uint count) {
  if (FeatureSupport::DSA_ARB) {
    glNamedBufferSubData(m_id, offset, count, data);
  } else if (FeatureSupport::DSA_EXT) {
    glNamedBufferSubDataEXT(m_id, offset, count, data);
  } else {
    BoundBufferSave<GL_ARRAY_BUFFER> save;
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, count, data);
  }
}

}
}
}
