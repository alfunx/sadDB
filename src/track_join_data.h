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

typedef std::pair<Key, Payload> Record;
typedef std::tuple<Key, Payload, Payload> JoinedRecord;
typedef std::tuple<Key, Id, relation_type> SendCommand;
typedef std::tuple<Key, Id, relation_type, Cost> KeyCost;

typedef std::multimap<Key, Payload> RelationMap;
typedef std::vector<Record> Relation;
typedef std::vector<JoinedRecord> JoinedRelation;
typedef std::map<Key, std::vector<KeyCost>> KeyCostMap;

struct TrackJoinData
{
	// phase 1
	RelationMap rel_R; // T_r
	RelationMap rel_S; // T_s

	// phase 2
	KeyCostMap key_cost_map; // T_r|s

	// phase 3
	std::multimap<Key, std::pair<Payload, Payload>> joined_rel;
};
typedef TrackJoinData TJD;

#endif  // SADDB_TRACK_JOIN_DATA_H_
