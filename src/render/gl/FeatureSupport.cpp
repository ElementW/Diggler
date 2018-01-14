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
  F::buffer_storage,
  F::debug;

void F::probe() {
#define has(ext) OpenGL::hasExtension(ext)
  VAO = has("GL_ARB_vertex_array_object") or has("GL_OES_vertex_array_object");
  DSA_ARB = has("GL_ARB_direct_state_access");
  DSA_EXT = has("GL_EXT_direct_state_access");
  DSA = DSA_ARB or DSA_EXT;
  shader_image_load_store = has("GL_ARB_shader_image_load_store");
  FBO_ARB = has("GL_ARB_framebuffer_object");
  buffer_storage = has("GL_ARB_buffer_storage");
  debug = has("GL_KHR_debug") or has("GL_ARB_debug_output") or has("GL_AMD_debug_output");
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
  feature(debug);
  return oss.str();
}
#undef feature

}
}
}
