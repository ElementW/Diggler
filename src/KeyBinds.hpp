#ifndef KEY_BINDS_HPP
#define KEY_BINDS_HPP
#include <string>

namespace Diggler {

class KeyBinds {
public:
	// Keys
	int forward, backward,
		left, right,
		jump,
		chat, chatTeam,
		gameMenu, playerList;
	
	KeyBinds();
	void loadDefaults();
	void load(const std::string &path);
	void save(const std::string &path) const;
};

}

#endif