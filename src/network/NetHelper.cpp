#include "NetHelper.hpp"

#include "msgtypes/Chat.hpp"
#include "../Game.hpp"
#include "../Player.hpp"
#include "../Server.hpp"

namespace Diggler {

using namespace Net;

namespace NetHelper {

void Broadcast(Game *G, const OutMessage &msg, Tfer tfer, Channels chan) {
  Broadcast(*G, msg, tfer, chan);
}

void Broadcast(Game &G, const OutMessage &msg, Tfer tfer, Channels chan) {
  for (Player &p : G.players) {
    G.S->H.send(*p.peer, msg, tfer, chan);
  }
}

void SendChat(Game *G, const std::string &str) {
  Net::MsgTypes::ChatSend cs;
  cs.msg = goodform::object {
    {"plaintext", str}
  };

  OutMessage msg;
  cs.writeToMsg(msg);
  G->H.send(*G->NS, msg, Tfer::Unseq);
}


}
}
