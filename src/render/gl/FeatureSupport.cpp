#include "FeatureSupport.hpp"

namespace Diggler {
namespace Render {
namespace gl {

using F = FeatureSupport;

bool
  F::VAO,
  F::DSA;

void F::probe() {
  VAO = epoxy_has_gl_extension("GL_ARB_vertex_array_object") or
        epoxy_has_gl_extension("GL_OES_vertex_array_object");
  DSA = epoxy_has_gl_extension("ARB_direct_state_access");
}

}
}
}
