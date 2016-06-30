#include "PlayerList.hpp"
#include <stdexcept>
#include "Game.hpp"

namespace Diggler {

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

Player& PlayerList::getById(uint32 id) {
  if (G->LP && id == G->LP->id)
    return *G->LP;
  for (auto it = begin();
    it != end(); ++it) {
    if (it->id == id) {
      return *it;
    }
  }
  throw std::out_of_range("Can't find player by ID");
}

Player &PlayerList::getByName(const std::string &name) {
  if (G->LP && name == G->LP->name)
    return *G->LP;
  for (auto it = begin();
    it != end(); ++it) {
    if (it->name.compare(name) == 0) {
      return *it;
    }
  }
  throw std::out_of_range("Can't find player by name");
}

Player &PlayerList::getByPeer(const Net::Peer &peer) {
  for (auto it = begin();
    it != end(); ++it) {
    if (it->P == peer) {
      return *it;
    }
  }
  throw std::out_of_range("Can't find player by Net::Peer");
}

}