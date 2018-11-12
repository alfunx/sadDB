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
typedef std::map<Key, std::vector<KeyCostId>> KeyCostMap; // T_r|s
typedef std::tuple<Key, Id, relation_type> KeyNodeService;
typedef std::vector<std::pair<Key, Payload>> RelationVector;
typedef std::vector<std::tuple<Key, Payload, Payload>> JoinedRelation;

struct TrackJoinData
{
	// phase 1
	Relation rel_R; // T_r
	Relation rel_S; // T_s

	// phase 2
	KeyCostMap key_cost_map; // T_r|s

	// phase 3
	std::multimap<Key, std::pair<Payload, Payload>> joined_rel;
};
typedef TrackJoinData TJD;

#endif  // SADDB_TRACK_JOIN_DATA_H_
