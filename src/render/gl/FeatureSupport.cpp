#include "FeatureSupport.hpp"

namespace Diggler {
namespace Render {
namespace gl {

using F = FeatureSupport;

bool F::VAO;

void F::probe() {
  VAO = epoxy_has_gl_extension("GL_ARB_vertex_array_object");
}

}
}
}
