#include "PlayerQuitHandler.hpp"

#include "../../Game.hpp"
#include "../../GameState.hpp"
#include "../../util/Log.hpp"

namespace Diggler {
namespace Net {
namespace Client {

using namespace Net::MsgTypes;
using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "CNH:PlayerQuit";

bool PlayerQuitHandler::handle(GameState &GS, InMessage &msg) {
  uint32 id = msg.readU32();
  try {
    Player *plr = GS.G->players.getBySessId(id);
    if (plr != nullptr) {
      Log(Debug, TAG) << plr->name << " is gone :(";
      GS.G->players.remove(*plr);
    }
  } catch (const std::out_of_range &e) {
    Log(Debug, TAG) << "Phantom player #" << id << " disconnected";
  }
  return true;
}

}
}
}
