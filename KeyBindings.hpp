#ifndef KEY_BINDINGS_HPP
#define KEY_BINDINGS_HPP
#include <map>
#include <string>

namespace Diggler {

class KeyBindings {
public:
	enum Bind {
		Forward,
		Backward,
		Left,
		Right,
		Jump,
		Sprint,
		PlayerList,
	};

private:
	std::map<Bind, int> m_bindings;

public:
	KeyBindings();
	void set(Bind bind, int key);
	int get(Bind bind);
	void save(const std::string &path) const;
	void load(const std::string &path);
};

}

#endif