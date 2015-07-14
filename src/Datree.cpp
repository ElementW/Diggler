#include "Datree.hpp"

namespace Diggler {

Datree::BadType::BadType() : logic_error("Bad type") {}
Datree::BadValueType::BadValueType() : logic_error("Bad value type") {}
Datree::NotInMap::NotInMap() : out_of_range("Not in map") {}


Datree::Datree(Type type) {
	setType(type);
}

Datree::Datree(NumberType val) {
	*this = val;
}

Datree::Datree(const Datree::StringType &val) {
	*this = val;
}


Datree::Datree(const Datree &o) {
	m_type = o.m_type;
	switch (o.m_type) {
		case Type::String:
			m_string = new StringType(*(o.m_string));
			break;
		case Type::Map:
			m_map = new MapType(*(o.m_map));
			break;
		case Type::Number:
			m_number = o.m_number;
			break;
		default:
			break;
	}
}

Datree& Datree::operator=(const Datree &o) {
	setType(o.m_type);
	switch (o.m_type) {
		case Type::String:
			*m_string = *(o.m_string);
			break;
		case Type::Map:
			*m_map = *(o.m_map);
			break;
		case Type::Number:
			m_number = o.m_number;
			break;
		default:
			break;
	}
	return *this;
}


Datree::Datree(Datree &&o) {
	(*this) = o;
}

Datree& Datree::operator=(Datree &&o) {
	m_type = o.m_type;
	switch (m_type) {
		case Type::Number:
			m_number = o.m_number;
			break;
		case Type::String:
		case Type::Map:
			m_map = o.m_map;
			o.m_map = nullptr;
			break;
		default:
			break;
	}
	return *this;
}


void Datree::setType(Type type) {
	if (m_type == type) {
		if (type == Type::Map)
			m_map->clear();
		return;
	}
	switch (m_type) {
		case Type::String:
			delete m_string;
			break;
		case Type::Map:
			delete m_map;
			break;
		default:
			break;
	}
	switch (type) {
		case Type::String:
			m_string = new StringType;
			break;
		case Type::Map:
			m_map = new MapType;
			break;
		case Type::Number:
			m_number = 0;
			break;
		default:
			break;
	}
}


Datree::~Datree() {
	switch (m_type) {
		case Type::String:
			delete m_string;
			break;
		case Type::Map:
			delete m_map;
			break;
		default:
			break;
	}
}


Datree::Type Datree::getType() const {
	return m_type;
}


Datree::NumberType Datree::getNumber() const {
	if (m_type != Type::Number)
		throw BadType();
	return m_number;
}

Datree::NumberType Datree::getNumber(NumberType fallback, bool set) {
	if (m_type != Type::Number) {
		if (set)
			*this = fallback;
		return fallback;
	}
	return m_number;
}

Datree& Datree::operator=(NumberType val) {
	setType(Type::Number);
	m_number = val;
	return *this;
}


Datree::StringType Datree::getString() const {
	if (m_type != Type::String)
		throw BadType();
	return *m_string;
}

Datree::StringType Datree::getString(const StringType &fallback, bool set) {
	if (m_type != Type::String) {
		if (set) {
			*this = fallback;
			return *m_string;
		}
		return fallback;
	}
	return *m_string;
}

Datree& Datree::operator=(const Datree::StringType &val) {
	setType(Type::String);
	*m_string = val;
	return *this;
}


uint Datree::getCount() {
	return m_map->size();
}

Datree& Datree::at(const StringType &key) {
	MapType::iterator it = m_map->find(key);
	if (it == m_map->end())
		throw NotInMap();
	return it->second;
}

Datree& Datree::at(const StringType &key, Datree &cVal, bool create) {
	MapType::iterator it = m_map->find(key);
	if (it == m_map->end()) {
		if (create) {
			it = (m_map->emplace(std::make_pair(key, cVal))).first;
			return it->second;
		}
		return cVal;
	}
	return it->second;
}

Datree::NumberType Datree::numberAt(const StringType &key) {
	Datree &dt = at(key);
	if (dt.m_type != Type::Number)
		throw BadValueType();
	return dt.m_number;
}

Datree::NumberType Datree::numberAt(const StringType &key, const NumberType &fallback, bool create) {
	MapType::iterator it = m_map->find(key);
	if (it == m_map->end()) {
		if (create)
			m_map->emplace(std::make_pair(key, fallback));
		return fallback;
	}
	Datree &dt = it->second;
	if (dt.m_type != Type::Number) {
		if (create)
			m_map->emplace(std::make_pair(key, fallback));
		return fallback;
	}
	return dt.m_number;
}

Datree::StringType Datree::stringAt(const StringType &key) {
	Datree &dt = at(key);
	if (dt.m_type != Type::String)
		throw BadValueType();
	return *dt.m_string;
}

Datree::StringType Datree::stringAt(const StringType &key, const StringType &fallback, bool create) {
	MapType::iterator it = m_map->find(key);
	if (it == m_map->end()) {
		if (create) {
			it = (m_map->emplace(std::make_pair(key, fallback))).first;
			return *it->second.m_string;
		}
		return fallback;
	}
	Datree &dt = it->second;
	if (dt.m_type != Type::String) {
		if (create) {
			it = (m_map->emplace(std::make_pair(key, fallback))).first;
			return *it->second.m_string;
		}
		return fallback;
	}
	return *dt.m_string;
}

void Datree::set(const StringType &key, const StringType &val) {
	MapType::iterator it = m_map->find(key);
	if (it == m_map->end()) {
		it->second = val;
	} else {
		m_map->emplace(std::make_pair(key, val));
	}
}

void Datree::set(const StringType &key, NumberType val) {
	MapType::iterator it = m_map->find(key);
	if (it == m_map->end()) {
		it->second = val;
	} else {
		m_map->emplace(std::make_pair(key, val));
	}
}

void Datree::set(const StringType &key, const Datree &val) {
	MapType::iterator it = m_map->find(key);
	if (it == m_map->end()) {
		it->second = val;
	} else {
		m_map->emplace(std::make_pair(key, val));
	}
}

}