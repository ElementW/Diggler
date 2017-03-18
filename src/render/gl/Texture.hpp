#ifndef DIGGLER_RENDER_GL_TEXTURE_HPP
#define DIGGLER_RENDER_GL_TEXTURE_HPP

#include <memory>
#include <string>

#include "../../Texture.hpp"
#include "OpenGL.hpp"

namespace Diggler {
namespace Render {
namespace gl {

class Texture : public Diggler::Texture {
private:
  // maybe ? TextureManager &TM;
  GLuint m_id;

public:
  Texture(uint w, uint h, PixelFormat format, const uint8 *data = nullptr);
  ~Texture();

  inline GLuint id() const { return m_id; }
  inline operator GLuint() const { return m_id; }

  void getTexture(uint8 *data) override;

  void setTexture(std::unique_ptr<const uint8[]> &&data, PixelFormat format) override;

  void setTexture(uint w, uint h, std::unique_ptr<const uint8[]> &&data,
      PixelFormat format) override;

  void setSubTexture(int x, int y, uint w, uint h, std::unique_ptr<const uint8[]> &&data,
      PixelFormat format) override;

  void setFiltering(Filter min, Filter mag) override;

  void setWrapping(Wrapping s, Wrapping t) override;
  using Diggler::Texture::setWrapping;

  void bind() const {
    glBindTexture(GL_TEXTURE_2D, m_id);
  }
};

}
}
}

#endif /* DIGGLER_RENDER_GL_TEXTURE_HPP */
