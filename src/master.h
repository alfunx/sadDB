#ifndef SADDB_MASTER_H_
#define SADDB_MASTER_H_

#include "node.h"
#include "relation.h"
#include "tcp_client.h"
#include "tcp_server.h"
#include "tcp_traits.h"
#include "track_join_data.h"

class Master
{

public:

	Node node;
	KeyCostMap key_cost_map;

	TCP_Server<KeyCost>* tcp_server_2;

	unsigned int phase_3_sent_messages_count = 0;

	std::vector<unsigned int> phase_3_sent_messages_sizes;

public:

	Master(Node& n);
	void phase_2();
	void phase_3();

private:

	typedef std::vector<std::pair<Address, int>> AddressCostList;
	unsigned int broadcast_cost(const AddressCostList& r, const AddressCostList& s);
	void broadcast(const KeyCostMap::iterator it, const Relation::Type t);

};

#endif  // SADDB_MASTER_H_
