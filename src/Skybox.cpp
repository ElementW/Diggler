#include "Skybox.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "render/gl/ProgramManager.hpp"
#include "Texture.hpp"

namespace Diggler {

const Render::gl::Program *Skybox::RenderProgram = nullptr;
GLint Skybox::RenderProgram_attrib_texcoord = -1;
GLint Skybox::RenderProgram_attrib_coord = -1;
GLint Skybox::RenderProgram_uni_mvp = -1;

Skybox::Skybox(Game *G, const std::string &skyName) : G(G) {
  if (RenderProgram == nullptr) {
    RenderProgram = G->PM->getProgram(PM_3D | PM_TEXTURED);
    RenderProgram_attrib_coord = RenderProgram->att("coord");
    RenderProgram_attrib_texcoord = RenderProgram->att("texcoord");
    RenderProgram_uni_mvp = RenderProgram->uni("mvp");
  }
  m_top = new Texture(skyName + "_up.png");
  m_bottom = new Texture(skyName + "_down.png");
  m_n = new Texture(skyName + "_north.png");
  m_n->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
  m_e = new Texture(skyName + "_east.png");
  m_e->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
  m_w = new Texture(skyName + "_west.png");
  m_w->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
  m_s = new Texture(skyName + "_south.png");
  m_s->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
  Coord coords[6*6] = {
    // Top
    { -1,  1, -1, 0, 0 },
    {  1,  1, -1, 1, 0 },
    { -1,  1,  1, 0, 1 },
    { -1,  1,  1, 0, 1 },
    {  1,  1, -1, 1, 0 },
    {  1,  1,  1, 1, 1 },
    // Bottom
    { -1, -1,  1, 0, 0 },
    {  1, -1, -1, 1, 1 },
    { -1, -1, -1, 0, 1 },
    {  1, -1,  1, 1, 0 },
    {  1, -1, -1, 1, 1 },
    { -1, -1,  1, 0, 0 },
    // East
    { -1, -1, -1, 0, 1 },
    { -1,  1, -1, 0, 0 },
    { -1, -1,  1, 1, 1 },
    { -1,  1,  1, 1, 0 },
    { -1, -1,  1, 1, 1 },
    { -1,  1, -1, 0, 0 },
    // West
    {  1,  1, -1, 1, 0 },
    {  1, -1, -1, 1, 1 },
    {  1, -1,  1, 0, 1 },
    {  1, -1,  1, 0, 1 },
    {  1,  1,  1, 0, 0 },
    {  1,  1, -1, 1, 0 },
    // North
    { -1,  1, -1, 1, 0 },
    { -1, -1, -1, 1, 1 },
    {  1, -1, -1, 0, 1 },
    {  1, -1, -1, 0, 1 },
    {  1,  1, -1, 0, 0 },
    { -1,  1, -1, 1, 0 },
    // South
    { -1, -1,  1, 0, 1 },
    { -1,  1,  1, 0, 0 },
    {  1, -1,  1, 1, 1 },
    {  1,  1,  1, 1, 0 },
    {  1, -1,  1, 1, 1 },
    { -1,  1,  1, 0, 0 },
  };
  m_vbo.setData(coords, 6*6);
}

void Skybox::render(const glm::mat4 &transform) const {
  RenderProgram->bind();
  m_vbo.bind();
  glEnableVertexAttribArray(RenderProgram_attrib_coord);
  glEnableVertexAttribArray(RenderProgram_attrib_texcoord);
  glUniformMatrix4fv(RenderProgram_uni_mvp, 1, GL_FALSE, glm::value_ptr(transform));
  glVertexAttribPointer(RenderProgram_attrib_coord, 3, GL_BYTE, GL_FALSE, sizeof(Coord), 0);
  glVertexAttribPointer(RenderProgram_attrib_texcoord, 2, GL_BYTE, GL_FALSE, sizeof(Coord), (GLvoid*)offsetof(Coord, u));
  
  m_top->bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
  m_bottom->bind();
  glDrawArrays(GL_TRIANGLES, 6, 6);
  m_e->bind();
  glDrawArrays(GL_TRIANGLES, 12, 6);
  m_w->bind();
  glDrawArrays(GL_TRIANGLES, 18, 6);
  m_n->bind();
  glDrawArrays(GL_TRIANGLES, 24, 6);
  m_s->bind();
  glDrawArrays(GL_TRIANGLES, 30, 6);
  
  glDisableVertexAttribArray(RenderProgram_attrib_texcoord);
  glDisableVertexAttribArray(RenderProgram_attrib_coord);
}

Skybox::~Skybox() {
  delete m_top;
}

}
