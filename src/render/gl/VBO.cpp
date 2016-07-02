#include "VBO.hpp"

namespace Diggler {
namespace Render {
namespace gl {

static void unref(int *rc, GLuint id) {
  if (--(*rc) == 0) {
    glDeleteBuffers(1, &id);
    delete rc;
  }
}

VBO::VBO() {
  *(m_refcount = new int) = 1;
  glGenBuffers(1, &id);
}

VBO::VBO(const VBO &other) {
  (*this) = other;
}
VBO& VBO::operator=(const VBO &other) {
  unref(m_refcount, id);
  id = other.id;
  m_refcount = other.m_refcount;
  (*m_refcount)++;
  return *this;
}

VBO::VBO(VBO &&other) {
  (*this) = other;
}
VBO& VBO::operator=(VBO &&other) {
  unref(m_refcount, id);
  id = other.id;
  m_refcount = other.m_refcount;
  return *this;
}

VBO::~VBO() {
  unref(m_refcount, id);
}

void VBO::bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, id);
}

int VBO::getSize() const {
  BoundBufferSave<GL_ARRAY_BUFFER> save;

  GLint data;
  bind();
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);

  return data;
}

void VBO::setSize(uint size, GLenum usage) {
  BoundBufferSave<GL_ARRAY_BUFFER> save;

  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, usage);
}

}
}
}
