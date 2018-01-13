#include "PlayerUpdateHandler.hpp"

#include "../../Game.hpp"
#include "../../util/Log.hpp"
#include "../msgtypes/PlayerUpdate.hpp"

namespace diggler {
namespace net {
namespace Client {

using namespace net::MsgTypes;
using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "CNH:PlayerUpdate";

bool PlayerUpdateHandler::handle(Game &G, InMessage &msg) {
  using S = PlayerUpdateSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Move: {
      PlayerUpdateMove pum;
      pum.readFromMsg(msg);
      if (!pum.plrSessId) {
        Log(Debug, TAG) << "Move without player session ID";
        return true;
      }
      Player *plr = G.players.getBySessId(*pum.plrSessId);
      if (!plr) {
        Log(Debug, TAG) <<"Move: sess#" << *pum.plrSessId <<
            " is not on server";
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
      Player *plr = G.players.getBySessId(pud.plrSessId);
      if (!plr) {
        Log(Debug, TAG) << "Die: sess#" << pud.plrSessId <<
            " is not on server";
        return true;
      }
      plr->setDead(false);
    } break;
    case S::Respawn: {
      PlayerUpdateRespawn pur;
      pur.readFromMsg(msg);
      Player *plr = G.players.getBySessId(pur.plrSessId);
      if (!plr) {
        Log(Debug, TAG) << "Respawn: sess#" << pur.plrSessId <<
            " is not on server";
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
