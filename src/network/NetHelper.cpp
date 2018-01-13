#include "NetHelper.hpp"

#include <meiose/variant.hpp>

#include "msgtypes/Chat.hpp"
#include "../Game.hpp"
#include "../Player.hpp"
#include "../Server.hpp"

namespace diggler {

using namespace net;

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
  net::MsgTypes::ChatSend cs;
  cs.msg = meiose::variant::map {
    {"plaintext", str}
  };

  OutMessage msg;
  cs.writeToMsg(msg);
  G->H.send(*G->NS, msg, Tfer::Unseq);
}


}
}
