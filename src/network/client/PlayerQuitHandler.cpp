#include "PlayerQuitHandler.hpp"

#include "../../Game.hpp"
#include "../../util/Log.hpp"

namespace diggler {
namespace net {
namespace Client {

using namespace net::MsgTypes;
using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "CNH:PlayerQuit";

bool PlayerQuitHandler::handle(Game &G, InMessage &msg) {
  uint32 id = msg.readU32();
  try {
    Player *plr = G.players.getBySessId(id);
    if (plr != nullptr) {
      Log(Debug, TAG) << plr->name << " is gone :(";
      G.players.remove(*plr);
    }
  } catch (const std::out_of_range &e) {
    Log(Debug, TAG) << "Phantom player #" << id << " disconnected";
  }
  return true;
}

}
}
}
