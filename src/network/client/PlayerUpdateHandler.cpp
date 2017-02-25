#include "PlayerUpdateHandler.hpp"

#include "../../Game.hpp"
#include "../../GameState.hpp"
#include "../msgtypes/PlayerUpdate.hpp"

namespace Diggler {
namespace Net {
namespace Client {

using namespace Net::MsgTypes;

bool PlayerUpdateHandler::handle(GameState &GS, InMessage &msg) {
  using S = PlayerUpdateSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Move: {
      PlayerUpdateMove pum;
      pum.readFromMsg(msg);
      if (!pum.plrSessId) {
        getOutputStream() << "PlayerUpdateMove without player session ID" << std::endl;
        return true;
      }
      Player *plr = GS.G->players.getBySessId(*pum.plrSessId);
      if (!plr) {
        getOutputStream() << "PlayerUpdateMove: sess#" << *pum.plrSessId <<
          " is not on server" << std::endl;
        return true;
      }
      glm::vec3 pos = pum.position ? *pum.position : plr->position,
                vel = pum.velocity ? *pum.velocity : plr->velocity,
                acc = pum.accel ? *pum.accel : plr->accel;
      plr->setPosVel(pos, vel, acc);
      plr->angle = pum.angle;
    } break;
    case S::Die: {
      PlayerUpdateDie pud;
      pud.readFromMsg(msg);
      Player *plr = GS.G->players.getBySessId(pud.plrSessId);
      if (!plr) {
        getOutputStream() << "PlayerUpdateDie: sess#" << pud.plrSessId <<
          " is not on server" << std::endl;
        return true;
      }
      plr->setDead(false);
    } break;
    case S::Respawn: {
      PlayerUpdateRespawn pur;
      pur.readFromMsg(msg);
      Player *plr = GS.G->players.getBySessId(pur.plrSessId);
      if (!plr) {
        getOutputStream() << "PlayerUpdateRespawn: sess#" << pur.plrSessId <<
          " is not on server" << std::endl;
        return true;
      }
      plr->setDead(false);
    } break;
    default:
      break;
  }
  return true;
}

}
}
}
