#ifndef PLAYER_LIST_HPP
#define PLAYER_LIST_HPP
#include "Platform.hpp"
#include <vector>
#include <string>
#include "Player.hpp"

namespace Diggler {

class Game;

class PlayerList : private std::vector<Player> {
  Game *G;
  PlayerList(const PlayerList&) = delete;
  PlayerList(PlayerList&) = delete;
  PlayerList& operator=(const PlayerList&) = delete;
  PlayerList& operator=(PlayerList&) = delete;

public:
  PlayerList(Game*);
  ~PlayerList();
  
  using std::vector<Player>::size;
  using std::vector<Player>::begin;
  using std::vector<Player>::end;

  using std::vector<Player>::operator[];

  /**
   * @brief Gets a Player using its game session ID
   * @return Pointer to Player, may be nullptr if not found
   */
  Player* getByGameId(uint32);

  /**
   * @brief Gets a Player using its name
   * @return Pointer to Player, may be nullptr if not found
   */
  Player* getByName(const std::string&);

  /**
   * @brief Gets a Player using its network peer object
   * @return Pointer to Player, may be nullptr if not found
   */
  Player* getByPeer(const Net::Peer&);

  Player& add();
  void remove(const Player&);
  void remove(int);
};

}

#endif
