#include "ChatHandler.hpp"

#include "../../Chatbox.hpp"
#include "../../Game.hpp"
#include "../../GameState.hpp"
#include "../msgtypes/Chat.hpp"

namespace Diggler {
namespace Net {
namespace Client {

using namespace Net::MsgTypes;

bool ChatHandler::handle(GameState &GS, InMessage &msg) {
  using S = ChatSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Send: {
      ; // No-op
    } break;
    case S::Announcement: {
      ChatAnnouncement ca;
      ca.readFromMsg(msg);
      // TODO better formatting abilities
      if (ca.msg.is<goodform::object>()) {
        GS.m_chatBox->addChatEntry(ca.msg["plaintext"].get<std::string>());
      }
    } break;
    case S::PlayerTalk: {
      ChatPlayerTalk cpt;
      cpt.readFromMsg(msg);
      // TODO better formatting abilities
      if (cpt.msg.is<std::string>()) {
        std::string playerName;
        if (cpt.player.display.is<std::nullptr_t>()) {
          const Player *blabbermouth = GS.G->players.getBySessId(cpt.player.id);
          if (blabbermouth != nullptr) {
            playerName = blabbermouth->name + "> ";
          } else {
            playerName = "?> ";
          }
        } else if (cpt.player.display.is<std::string>()) {
          cpt.player.display.get<std::string>(playerName);
        }
        GS.m_chatBox->addChatEntry(playerName + cpt.msg.get<std::string>());
      }
    } break;
  }
  return true;
}

}
}
}
