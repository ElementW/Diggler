#include "FeatureSupport.hpp"

#include <sstream>

#include "OpenGL.hpp"

namespace diggler {
namespace render {
namespace gl {

using F = FeatureSupport;

bool
  F::VAO,
  F::DSA_ARB,
  F::DSA_EXT,
  F::DSA,
  F::shader_image_load_store,
  F::FBO_ARB,
  F::buffer_storage;

void F::probe() {
  VAO = OpenGL::hasExtension("GL_ARB_vertex_array_object") or
        OpenGL::hasExtension("GL_OES_vertex_array_object");
  DSA_ARB = OpenGL::hasExtension("GL_ARB_direct_state_access");
  DSA_EXT = OpenGL::hasExtension("GL_EXT_direct_state_access");
  DSA = DSA_ARB or DSA_EXT;
  shader_image_load_store = OpenGL::hasExtension("GL_ARB_shader_image_load_store");
  FBO_ARB = OpenGL::hasExtension("GL_ARB_framebuffer_object");
  buffer_storage = OpenGL::hasExtension("GL_ARB_buffer_storage");
}

#define feature(x) if(x){oss<<#x<<std::endl;}
std::string F::supported() {
  std::ostringstream oss;
  feature(VAO);
  feature(DSA_ARB);
  feature(DSA_EXT);
  feature(DSA);
  feature(shader_image_load_store);
  feature(FBO_ARB);
  feature(buffer_storage);
  return oss.str();
}
#undef feature

}
}
}
