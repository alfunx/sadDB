#ifndef SADDB_PHASE_2_H_
#define SADDB_PHASE_2_H_

#include <iostream>

#include "enums.h"
#include "node.h"
#include "phase.h"
#include "serialize_tuple.h"
#include "tcp_traits.h"
#include "track_join_data.h"

namespace phase2
{

TCP_Server<KeyCostId>* tcp_server;

KeyCostMap key_cost_map;

void master(Node& node, KeyCostMap& kc)
{
	TCP_Server<KeyCostId> t = TCP_Server<KeyCostId>(node.port() + 1, [&kc](boost::shared_ptr<KeyCostId> p, ConnectionPtr c) {
		kc[std::get<0>(*p)].push_back(*p);
	});
	tcp_server = &t;
	tcp_server->start();
}

void slave(Node& node, relation_type type, Relation& rel)
{
	// TODO remove
	std::cout << "sizes: " << rel.size() << std::endl;

	// iterate over distinct key
	for (auto it = rel.begin(), end = rel.end();
			it != end;
			it = rel.upper_bound(it->first))
	{
		KeyCostId t(it->first, node.id(), rel.count(it->first), type);

		// TODO send x (key, count) to processT (hash(k) mod N)
		unsigned int n = std::get<0>(t) % node.servers()->size() + 1;
		Address address;

		for (auto a : *node.servers())
		{
			if (a.id() == n)
			{
				address = a;
				break;
			}
		}

		// TODO remove
		std::cout << type << " -- "
			<< std::get<0>(t) << ":"
			<< std::get<2>(t) << ", "
			<< "rel:" << std::get<3>(t) << ", "
			<< "to: " << address << std::endl;

		TCP_Client<KeyCostId> tcp_client(t, address.ip(), address.port() + 1);
		tcp_client.start();
	}
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

		boost::thread process_t {
			boost::bind(&phase2::master, boost::ref(node_), boost::ref(phase2::key_cost_map))
		};

		boost::thread process_r {
			boost::bind(&phase2::slave, boost::ref(node_), R, boost::ref(data.rel_R))
		};

		boost::thread process_s {
			boost::bind(&phase2::slave, boost::ref(node_), S, boost::ref(data.rel_S))
		};

		process_r.join();
		process_s.join();

		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate(TJD& data)
	{
		phase2::tcp_server->stop();

		// TODO remove
		sleep(1);

		// set permanent
		data.key_cost_map = phase2::key_cost_map;

		// TODO remove
		for (auto v : data.key_cost_map)
		{
			for (auto e : v.second)
				std::cout << std::get<0>(e) << ","
					<< std::get<1>(e) << ","
					<< std::get<2>(e) << std::endl;
		}
	}

};

#endif  // SADDB_PHASE_2_H_
