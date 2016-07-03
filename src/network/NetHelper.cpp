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
    G.S->H.send(p.P, msg, tfer, chan);
  }
}

void SendChat(Game *G, const std::string &str) {
  Net::MsgTypes::ChatSend cs;
  cs.msg = msgpack::object(str, cs.z);

  OutMessage msg;
  cs.writeToMsg(msg);
  G->H.send(G->NS, msg, Tfer::Unseq);
}

void SendToolUse(Game *G, int x, int y, int z) {
  OutMessage msg(MessageType::PlayerUpdate, PlayerUpdateType::ToolUse);
  msg.writeBool(true);
  msg.writeIVec3(x, y, z);
  G->H.send(G->NS, msg, Tfer::Rel, Channels::PlayerInteract);
}

void SendToolUse(Game *G) {
  OutMessage msg(MessageType::PlayerUpdate, PlayerUpdateType::ToolUse);
  msg.writeBool(false);
  G->H.send(G->NS, msg, Tfer::Rel, Channels::PlayerInteract);
}


}
}
