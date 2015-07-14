#ifndef DATREE_HPP
#define DATREE_HPP
#include <stdexcept>
#include <map>
#include "Platform.hpp"

namespace Diggler {

class Datree {
public:
	typedef double NumberType;
	typedef std::string StringType;
	typedef std::map<StringType, Datree> MapType;

	enum class Type : uint8 {
		Number,
		String,
		Map
	};

private:
	Type m_type;
	union {
		NumberType m_number;
		StringType *m_string;
		MapType *m_map;
	};
	void setType(Type);

public:
	class BadType : public std::logic_error {
	public:
		BadType();
	};

	class BadValueType : public std::logic_error {
	public:
		BadValueType();
	};

	class NotInMap : public std::out_of_range {
	public:
		NotInMap();
	};

	Datree(Type type = Type::Number);
	Datree(NumberType val);
	Datree(const StringType &val);

	Datree(const Datree&);
	Datree& operator=(const Datree&);

	Datree(Datree&&);
	Datree& operator=(Datree&&);

	~Datree();

	Type getType() const;

	/* ============ Type: number ============ */

	NumberType getNumber() const;
	NumberType getNumber(NumberType fallback, bool set = false);

	Datree& operator=(NumberType val);

	/* ============ Type: string ============ */

	StringType getString() const;
	StringType getString(const StringType &fallback, bool set = false);

	Datree& operator=(const StringType &val);

	/* ============ Type: map ============ */

	uint getCount();

	Datree& at(const StringType &key);
	Datree& at(const StringType &key, Datree &cVal, bool create = false);

	NumberType numberAt(const StringType &key);
	NumberType numberAt(const StringType &key, const NumberType &fallback, bool create = false);

	StringType stringAt(const StringType &key);
	StringType stringAt(const StringType &key, const StringType &fallback, bool create = false);

	void set(const StringType &key, NumberType val);
	void set(const StringType &key, const StringType &val);
	void set(const StringType &key, const Datree &val);
};

}

#endif