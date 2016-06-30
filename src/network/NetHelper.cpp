#include "NetHelper.hpp"
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

void MakeEvent(OutMessage &msg, EventType t, const Player &p) {
  msg.setType(MessageType::Event);
  msg.setSubtype(t);
  msg.writeU32(p.id);
}

void MakeEvent(OutMessage &msg, EventType t, const glm::vec3 &p) {
  msg.setType(MessageType::Event);
  msg.setSubtype(t);
  msg.writeVec3(p);
}

void SendChat(Game *G, const std::string &str) {
  OutMessage chat(MessageType::Chat);
  chat.writeString(str);
  G->H.send(G->NS, chat, Tfer::Unseq);
}

void SendEvent(Game *G, EventType t) {
  OutMessage msg(MessageType::Event, t);
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