#include "PlayerJoinHandler.hpp"

#include "../../Game.hpp"
#include "../../GameState.hpp"
#include "../msgtypes/PlayerJoin.hpp"

namespace Diggler {
namespace Net {
namespace Client {

using namespace Net::MsgTypes;

bool PlayerJoinHandler::handle(GameState &GS, InMessage &msg) {
  using S = PlayerJoinSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Broadcast: {
      PlayerJoinBroadcast pjb;
      pjb.readFromMsg(msg);
      Player &plr = GS.G->players.add();
      plr.sessId = pjb.sessId;
      plr.name = pjb.name;
      getDebugStream() << "Player " << pjb.name <<
        '(' << pjb.sessId << ") joined the party!" << std::endl;
    } break;
    default: {
      // No-op
    } break;
  }
  return true;
}

}
}
}
