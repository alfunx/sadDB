#ifndef SADDB_TRACK_JOIN_DATA_H_
#define SADDB_TRACK_JOIN_DATA_H_

#include <map>
#include <vector>

#include "boost/serialization/map.hpp"

#include "enums.h"

typedef unsigned int Id;
typedef int Key;
typedef int Cost;
typedef std::string Payload;

typedef std::multimap<Key, Payload> Relation;
typedef std::tuple<Key, Id, Cost, relation_type> KeyCostId;
typedef std::tuple<Id, Cost, relation_type> CostId;
typedef std::multimap<Key, Id> send_to;
typedef std::multimap<Key, Payload> send_tuples;

struct TrackJoinData
{
	// phase 1
	Relation rel_R;
	Relation rel_S;

	// phase 2
	std::vector<KeyCostId> key_cost_id; // T_r|s

	// phase 3
	std::multimap<Key, std::pair<Payload, Payload>> joined_rel;
};
typedef TrackJoinData TJD;

#endif  // SADDB_TRACK_JOIN_DATA_H_
