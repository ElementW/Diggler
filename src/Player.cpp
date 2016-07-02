#include "Texture.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "GlobalProperties.hpp"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace Diggler {

Player::Renderer Player::R = {0};

Player::Player(Game *G) :
  G(G), position(0), velocity(0), accel(0), angle(0), toolUseTime(0),
  isAlive(true) {
  if (GlobalProperties::IsClient) {
    if (R.prog == nullptr) {
      R.prog = G->PM->getProgram(PM_3D | PM_FOG);
      R.att_coord = R.prog->att("coord");
      R.uni_mvp = R.prog->uni("mvp");
      R.uni_unicolor = R.prog->uni("unicolor");
      R.uni_fogStart = R.prog->uni("fogStart");
      R.uni_fogEnd = R.prog->uni("fogEnd");
      const float sz = .9f, szH = (sz*2);
      const float coords[6*3] = {
        -sz, .0f, 0.0f,
         sz, .0f, 0.0f,
         sz, szH, 0.0f,

        -sz, szH, 0.0f,
        -sz, .0f, 0.0f,
         sz, szH, 0.0f,
      };
      R.vbo = new Render::gl::VBO;
      R.vbo->setData(coords, 6*3);
    }
  }
}

using std::swap;
Player::Player(Player &&p) {
  *this = std::move(p);
}

Player& Player::operator=(Player &&p) {
  swap(direction, p.direction);
  swap(G, p.G);
  swap(position, p.position);
  swap(velocity, p.velocity);
  swap(accel, p.accel);
  swap(name, p.name);
  swap(id, p.id);
  swap(P, p.P);
  swap(isAlive, p.isAlive);
  return *this;
}

Player::~Player() {
  delete R.vbo;
}

void Player::setPosVel(const glm::vec3 &pos, const glm::vec3 &vel, const glm::vec3 &acc) {
  lastPosition = m_predictPos;
  position = m_predictPos = pos;
  velocity = vel;
  accel = acc;
  m_lastPosTime = G->Time;
}

void Player::update(const float &delta) {
  velocity += accel * delta;
  m_predictPos = glm::mix(lastPosition, position, std::min((G->Time-m_lastPosTime)*G->PlayerPosUpdateFreq, 1.0));
}

static inline int getSide(float angle) {
  if (angle >= 2*M_PI-M_PI/4 || angle < M_PI/4)
    return 1; // Back
  if (angle >= M_PI/4 && angle < M_PI-M_PI/4)
    return 2; // Left
  if (angle >= M_PI-M_PI/4 && angle >= M_PI+M_PI/4)
    return 0; // Right
  return 3; // Front
}

void Player::render(const glm::mat4 &transform) const {
  R.prog->bind();
  R.vbo->bind();
  glEnableVertexAttribArray(R.att_coord);
  glUniform4f(R.uni_unicolor, 1.f, 0.f, 0.f, 1.f);
  glUniform1f(R.uni_fogStart, G->RP->fogStart);
  glUniform1f(R.uni_fogEnd, G->RP->fogEnd);
  glm::vec3 &lpPos = G->LP->position;
  float angle = atan2(lpPos.x-m_predictPos.x, lpPos.z-m_predictPos.z);
  static const glm::vec3 vecY(0.0, 1.0, 0.0);
  glUniformMatrix4fv(R.uni_mvp, 1, GL_FALSE, glm::value_ptr(
    glm::translate(transform, m_predictPos) * glm::rotate(angle, vecY)));
  glVertexAttribPointer(R.att_coord, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDisableVertexAttribArray(R.att_coord);
}

void Player::setDead(bool dead, DeathReason dr, bool send) {
  isAlive = !dead;
  deathReason = dr;
}

}
