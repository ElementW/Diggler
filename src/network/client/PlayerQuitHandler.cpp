#include "PlayerQuitHandler.hpp"

#include "../../Game.hpp"
#include "../../GameState.hpp"

namespace Diggler {
namespace Net {
namespace Client {

using namespace Net::MsgTypes;

bool PlayerQuitHandler::handle(GameState &GS, InMessage &msg) {
  uint32 id = msg.readU32();
  try {
    Player *plr = GS.G->players.getBySessId(id);
    if (plr != nullptr) {
      getOutputStream() << plr->name << " is gone :(" << std::endl;
      GS.G->players.remove(*plr);
    }
  } catch (const std::out_of_range &e) {
    getOutputStream() << "Phantom player #" << id << " disconnected" << std::endl;
  }
  return true;
}

}
}
}
