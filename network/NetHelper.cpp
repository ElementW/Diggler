#include "NetHelper.hpp"
#include "../Game.hpp"
#include "../Player.hpp"
#include "../Server.hpp"

namespace Diggler {
namespace NetHelper {

void Broadcast(Game *G, const Net::OutMessage &msg, Net::Tfer tfer, Net::Channels chan) {
	Broadcast(*G, msg, tfer, chan);
}

void Broadcast(Game &G, const Net::OutMessage &msg, Net::Tfer tfer, Net::Channels chan) {
	for (Player &p : G.players) {
		G.S->H.send(p.P, msg, tfer, chan);
	}
}

void MakeEvent(Net::OutMessage &msg, Net::EventType t, const Player &p) {
	msg.setType(Net::MessageType::Event);
	msg.setSubtype(t);
	msg.writeU32(p.id);
}

void MakeEvent(Net::OutMessage &msg, Net::EventType t, const glm::vec3 &p) {
	msg.setType(Net::MessageType::Event);
	msg.setSubtype(t);
	msg.writeVec3(p);
}

void SendChat(Game *G, const std::string &str) {
	Net::OutMessage chat(Net::MessageType::Chat);
	chat.writeString(str);
	G->H.send(G->NS, chat, Net::Tfer::Unseq);
}

void SendEvent(Game* G, Net::EventType t) {
	Net::OutMessage msg(Net::MessageType::Event, t);
	G->H.send(G->NS, msg, Net::Tfer::Unseq);
}

}
}