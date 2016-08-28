#include "FeatureSupport.hpp"

#include <sstream>

namespace Diggler {
namespace Render {
namespace gl {

using F = FeatureSupport;

bool
  F::VAO,
  F::DSA_ARB,
  F::DSA_EXT,
  F::DSA,
  F::shader_image_load_store,
  F::FBO_ARB;

void F::probe() {
  VAO = epoxy_has_gl_extension("GL_ARB_vertex_array_object") or
        epoxy_has_gl_extension("GL_OES_vertex_array_object");
  DSA_ARB = epoxy_has_gl_extension("GL_ARB_direct_state_access");
  DSA_EXT = epoxy_has_gl_extension("GL_EXT_direct_state_access");
  DSA = DSA_ARB or DSA_EXT;
  shader_image_load_store = epoxy_has_gl_extension("GL_ARB_shader_image_load_store");
  FBO_ARB = epoxy_has_gl_extension("GL_ARB_framebuffer_object");
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
  return oss.str();
}
#undef feature

}
}
}
