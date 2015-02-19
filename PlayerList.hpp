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

	Player& getById(uint32);
	Player& getByName(const std::string&);
	Player& getByPeer(const Net::Peer&);

	Player& add();
	void remove(const Player&);
	void remove(int);
};

}

#endif