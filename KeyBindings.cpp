#include "KeyBindings.hpp"
#include "Platform.hpp"
#include <cstdio>
#include <stdexcept>

using std::fopen;
using std::fwrite;
using std::fread;
using std::fclose;

namespace Diggler {

KeyBindings::KeyBindings() {

}

void KeyBindings::load(const std::string &path) {
	FILE *f = fopen(path.c_str(), "r");
	if (!f)
		return;
	int32 count; fread(&count, sizeof(count), 1, f);
	for (int i=0; i < count; i++) {
		Bind bind; fread(&bind, sizeof(Bind), 1, f);
		int key; fread(&key, sizeof(key), 1, f); // Ambiguous int size, but GLFW uses ints as-is
		auto found = m_bindings.find(bind);
		if (found != m_bindings.end())
			m_bindings.erase(found);
		m_bindings.insert(std::pair<Bind, int>(bind, key));
	}
	fclose(f);
}

void KeyBindings::save(const std::string &path) const {
	FILE *f = fopen(path.c_str(), "w");
	if (!f)
		return;
	int32 count = m_bindings.size(); fwrite(&count, sizeof(count), 1, f);
	for (auto iter=m_bindings.begin(); iter != m_bindings.end(); iter++) {
		fwrite(&iter->first, sizeof(iter->first), 1, f);
		fwrite(&iter->second, sizeof(iter->second), 1, f);
	}
	fclose(f);
}

int KeyBindings::get(Bind bind) {
	try {
		return m_bindings.at(bind);
	} catch (const std::out_of_range &e) {
		return -1;
	}
}

void KeyBindings::set(Bind bind, int key) {
	auto found = m_bindings.find(bind);
	if (found != m_bindings.end())
		m_bindings.erase(found);
	m_bindings.insert(std::pair<Bind, int>(bind, key));
}

}