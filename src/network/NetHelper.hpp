#ifndef NET_HELPER_HPP
#define NET_HELPER_HPP
#include <string>
#include "Network.hpp"

namespace diggler {

class Player;
class Game;

namespace NetHelper {

// Server only
void Broadcast(Game*, const net::OutMessage&, net::Tfer = net::Tfer::Rel, net::Channels = net::Channels::Base);
void Broadcast(Game&, const net::OutMessage&, net::Tfer = net::Tfer::Rel, net::Channels = net::Channels::Base);
void MakeEvent(net::OutMessage&, net::EventType, const glm::vec3&);
void MakeEvent(net::OutMessage&, net::EventType, const Player&);

// Client only
void SendEvent(Game*, net::EventType);

void SendChat(Game*, const std::string&);

}
}

#endif
