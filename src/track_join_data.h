#ifndef SADDB_TRACK_JOIN_DATA_H_
#define SADDB_TRACK_JOIN_DATA_H_

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "boost/serialization/map.hpp"
#include "boost/lexical_cast.hpp"

#include "relation.h"

typedef unsigned int Id;
typedef int Key;
typedef int Cost;
typedef std::string Payload;

struct SendCommand
{

	Key key;
	Id id;

	SendCommand()
	{
		/* void */
	}

	SendCommand(Key k, Id i) :
		key(k),
		id(i)
	{
		/* void */
	}

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & key;
		ar & id;
	}

	friend std::ostream& operator <<(std::ostream& os, SendCommand r)
	{
		os << r.key << "," << r.id;
		return os;
	}

};

struct KeyCost : public SendCommand
{

	Relation::Type type;
	Cost cost;

	KeyCost()
	{
		/* void */
	}

	KeyCost(Key k, Id i, Relation::Type t, Cost c) :
		SendCommand(k, i),
		type(t),
		cost(c)
	{
		/* void */
	}

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & key;
		ar & id;
		ar & type;
		ar & cost;
	}

	friend std::ostream& operator <<(std::ostream& os, KeyCost r)
	{
		os << r.key << "," << r.id << "," << r.type << "," << r.cost;
		return os;
	}

};

typedef std::map<Key, std::vector<KeyCost>> KeyCostMap;

struct TrackJoinData
{

	// phase 1
	Relation rel_R; // T_r
	Relation rel_S; // T_s

	// phase 2
	KeyCostMap key_cost_map; // T_r|s

};
typedef TrackJoinData TJD;

#endif  // SADDB_TRACK_JOIN_DATA_H_
