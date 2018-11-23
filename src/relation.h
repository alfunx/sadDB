#ifndef SADDB_RELATION_H_
#define SADDB_RELATION_H_

#include <set>
#include <string>
#include <vector>

#include <boost/serialization/map.hpp>
#include <boost/lexical_cast.hpp>

#include "record.h"

typedef int Key;
typedef std::string Payload;

class Relation
{

	std::vector<Record> records;

public:

	Relation();
	Relation(std::vector<Record> r);

	Relation copy() const;
	void insert(const Record& r);
	void join(const Relation& a);
	void union_all(const Relation& a);
	void select(int k);
	void select(const std::set<int>& k);
	unsigned int count_keys(int k) const;
	unsigned int count() const;
	std::set<int> distinct_keys() const;

public:

	static Relation join(const Relation& a, const Relation& b);
	static Relation union_all(const Relation& a, const Relation& b);
	static Relation select(const Relation& a, int k);
	static Relation select(const Relation& a, const std::set<int>& k);

public:

	friend std::ostream& operator <<(std::ostream& os, Relation r);
	friend std::istream& operator >>(std::istream& is, Relation& r);

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & records;
	}

public:

	enum Type
	{
		R,
		S
	};

	static Type other(Type type);

};

#endif  // SADDB_RELATION_H_
