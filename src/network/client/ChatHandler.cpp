#include "ChatHandler.hpp"

#include "../../Chatbox.hpp"
#include "../../Game.hpp"
#include "../../states/GameState.hpp"
#include "../msgtypes/Chat.hpp"

namespace diggler {
namespace net {
namespace Client {

using namespace net::MsgTypes;

bool ChatHandler::handle(Game &G, InMessage &msg) {
  // TODO: get rid of
  states::GameState &GS = static_cast<states::GameState&>(G.GW->state());
  using S = ChatSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Send: {
      ; // No-op
    } break;
    case S::Announcement: {
      ChatAnnouncement ca;
      ca.readFromMsg(msg);
      // TODO better formatting abilities
      if (ca.msg.isMap()) {
        GS.m_chatBox->addChatEntry(ca.msg["plaintext"].getStr());
      }
    } break;
    case S::PlayerTalk: {
      ChatPlayerTalk cpt;
      cpt.readFromMsg(msg);
      // TODO better formatting abilities
      if (cpt.msg.isStr()) {
        std::string playerName;
        if (cpt.player.display.isNil()) {
          const Player *blabbermouth = G.players.getBySessId(cpt.player.id);
          if (blabbermouth != nullptr) {
            playerName = blabbermouth->name + "> ";
          } else {
            playerName = "?> ";
          }
        } else if (cpt.player.display.isStr()) {
          playerName = cpt.player.display.getStr();
        }
        GS.m_chatBox->addChatEntry(playerName + cpt.msg.getStr());
      }
    } break;
  }
  return true;
}

}
}
}
