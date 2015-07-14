#ifndef NET_HELPER_HPP
#define NET_HELPER_HPP
#include <string>
#include "Network.hpp"

namespace Diggler {

class Player;
class Game;

namespace NetHelper {

// Server only
void Broadcast(Game*, const Net::OutMessage&, Net::Tfer = Net::Tfer::Rel, Net::Channels = Net::Channels::Base);
void Broadcast(Game&, const Net::OutMessage&, Net::Tfer = Net::Tfer::Rel, Net::Channels = Net::Channels::Base);
void MakeEvent(Net::OutMessage&, Net::EventType, const glm::vec3&);
void MakeEvent(Net::OutMessage&, Net::EventType, const Player&);

// Client only
void SendEvent(Game*, Net::EventType);

void SendChat(Game*, const std::string&);
void SendToolUse(Game*);
void SendToolUse(Game*, int x, int y, int z);

}
}

#endif