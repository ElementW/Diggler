#ifndef DIGGLER_GFX_COMMAND_HPP
#define DIGGLER_GFX_COMMAND_HPP

#include <future>

#include "../platform/Types.hpp"

namespace diggler {
namespace gfx {

struct Command {
  enum class Class : uint8 {
    CommandBuffer,
    RenderPass,
    Framebuffer,
    ShaderModule,
    Pipeline,
    Buffer,
    Texture,
    Clear,
    Draw,
  } klass;
  uint8 cmd;
};

template<typename T>
struct ReturnCommand : public Command {
  std::promise<T> returnValue;
};

namespace cmd {
struct Draw : public Command {
  enum class Opacity {
    Opaque,
    Translucent,
    Transparent
  } opacity = Opacity::Opaque; // Reorderability

};
}

}
}

#endif /* DIGGLER_GFX_COMMAND_HPP */
