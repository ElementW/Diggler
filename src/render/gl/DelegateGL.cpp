#include "DelegateGL.hpp"

#include "Util.hpp"

#include "../../util/BitmapDumper.hpp"

namespace diggler {
namespace render {
namespace gl {

DelegateGL DelegateGL::instance;
std::thread::id DelegateGL::GLThreadId;

void DelegateGL::texImage2D(GLuint texture, GLint level, GLint internalformat, GLsizei width,
    GLsizei height, GLenum format, GLenum type, std::unique_ptr<const uint8[]> &&data) {
  if (GLThreadId == std::this_thread::get_id()) {
    Bound2DTextureSave save;
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, level, internalformat, width, height, 0, format, type, data.get());
  } else {
    push([=, data { std::move(data) }] {
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, level, internalformat, width, height, 0, format, type,
          data.get());
    });
  }
}

void DelegateGL::texSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset,
    GLsizei width, GLsizei height, GLenum format, GLenum type,
    std::unique_ptr<const uint8[]> &&data) {
  if (GLThreadId == std::this_thread::get_id()) {
    Bound2DTextureSave save;
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type,
        data.get());
  } else {
    push([=, data { std::move(data) }] {
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type,
          data.get());
    });
  }
}

void DelegateGL::execute() {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  { std::lock_guard<std::mutex> lock(instance.operationsMutex);
    for (Util::unique_function<void()> &func : instance.operations) {
      func();
    }
    instance.operations.clear();
  }

  // State reset
  glBindTexture(GL_TEXTURE_2D, 0);
}

}
}
}
