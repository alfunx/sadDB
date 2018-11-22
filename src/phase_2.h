#ifndef SADDB_PHASE_2_H_
#define SADDB_PHASE_2_H_

#include <iostream>

#include "node.h"
#include "phase.h"
#include "relation.h"
#include "serialize_tuple.h"
#include "tcp_traits.h"
#include "track_join_data.h"

namespace phase2
{

boost::thread process_t;
boost::thread process_r;
boost::thread process_s;

TCP_Server<KeyCost>* tcp_server;

KeyCostMap key_cost_map;

void master(Node& node, KeyCostMap& kc)
{
	TCP_Server<KeyCost> t = TCP_Server<KeyCost>(node.port() + 1,
			[&kc](boost::shared_ptr<KeyCost> p, ConnectionPtr c) {
		kc[p->key].push_back(*p);
	});
	tcp_server = &t;
	tcp_server->start();
}

void slave(Node& node, Relation::Type type, Relation& rel)
{
	auto keys = rel.distinct_keys();

	// iterate over distinct key
	for (auto key : keys)
	{
		KeyCost cost(key, node.id(), type, rel.count_keys(key));

		// TODO send x (key, count) to processT (hash(k) mod N)
		unsigned int id = key % node.servers()->size() + 1;
		Address address = node.get_address(id);

		// TODO remove
		std::cout << "- " << cost
			<< " - to: " << address << std::endl;

		TCP_Client<KeyCost> tcp_client(cost, address.ip(), address.port() + 1);
		tcp_client.start();
	}
}

class Phase_2 : public Phase
{

public:

	Phase_2(Node& node) :
		Phase(node)
	{
		/* void */
	}

	virtual void execute(TJD& data)
	{
		std::cout << "Processing: Phase 2" << std::endl;

		process_t = boost::thread {
			boost::bind(&master,
					boost::ref(node_),
					boost::ref(key_cost_map))
		};

		process_r = boost::thread {
			boost::bind(&slave,
					boost::ref(node_),
					Relation::Type::R,
					boost::ref(data.rel_R))
		};

		process_s = boost::thread {
			boost::bind(&slave,
					boost::ref(node_),
					Relation::Type::S,
					boost::ref(data.rel_S))
		};

		// wait for threads
		process_r.join();
		process_s.join();

		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate(TJD& data)
	{
		tcp_server->stop();

		// wait for thread
		process_t.join();

		// set permanent
		data.key_cost_map = key_cost_map;

		// TODO remove
		for (auto kv : data.key_cost_map)
		{
			for (auto t : kv.second)
				std::cout << t << std::endl;
		}
	}

};

}

using phase2::Phase_2;

#endif  // SADDB_PHASE_2_H_
