#ifndef DIGGLER_RENDER_RENDER_PARAMS_HPP
#define DIGGLER_RENDER_RENDER_PARAMS_HPP

#include <glm/mat4x4.hpp>

#include "../Frustum.hpp"
#include "../World.hpp"

namespace diggler {
namespace render {

class RenderParams {
public:
  glm::mat4 transform;
  Frustum frustum;

  World *world;
};

}
}

#endif /* DIGGLER_RENDER_RENDER_PARAMS_HPP */
