#include "PlayerList.hpp"

#include <stdexcept>

#include "Game.hpp"
#include "LocalPlayer.hpp"

namespace diggler {

PlayerList::PlayerList(Game *G) : G(G) {
}

PlayerList::~PlayerList() {
}

Player& PlayerList::add() {
  emplace_back(G);
  return back();
}

void PlayerList::remove(int idx) {
  erase(begin() + idx);
}

void PlayerList::remove(const Player &plr) {
  for (auto it = begin();
    it != end(); ++it) {
    if (&plr == &*it) {
      erase(it);
      return;
    }
  }
  // Actually bad
  throw std::out_of_range("Can't remove player: not in list");
}

Player* PlayerList::getBySessId(Player::SessionID sid) {
  if (G->LP && sid == G->LP->sessId)
    return G->LP;
  for (auto it = begin();
    it != end(); ++it) {
    if (it->sessId == sid) {
      return &*it;
    }
  }
  return nullptr;
}

Player* PlayerList::getByName(const std::string &name) {
  if (G->LP && name == G->LP->name)
    return G->LP;
  for (auto it = begin();
    it != end(); ++it) {
    if (it->name.compare(name) == 0) {
      return &*it;
    }
  }
  return nullptr;
}

Player* PlayerList::getByPeer(const net::Peer &peer) {
  for (auto it = begin();
    it != end(); ++it) {
    if (*it->peer == peer) {
      return &*it;
    }
  }
  return nullptr;
}

}
