#include "PlayerJoinHandler.hpp"

#include "../../Game.hpp"
#include "../../util/Log.hpp"
#include "../msgtypes/PlayerJoin.hpp"

namespace diggler {
namespace net {
namespace Client {

using namespace net::MsgTypes;
using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "CNH:PlayerJoin";

bool PlayerJoinHandler::handle(Game &G, InMessage &msg) {
  using S = PlayerJoinSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Broadcast: {
      PlayerJoinBroadcast pjb;
      pjb.readFromMsg(msg);
      Player &plr = G.players.add();
      plr.sessId = pjb.sessId;
      plr.name = pjb.name;
      Log(Info, TAG) << "Player " << pjb.name << '(' << pjb.sessId << ") joined the party!";
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
