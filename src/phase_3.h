#ifndef SADDB_PHASE_3_H_
#define SADDB_PHASE_3_H_

#include <iostream>

#include "enums.h"
#include "node.h"
#include "phase.h"
#include "serialize_tuple.h"
#include "tcp_traits.h"
#include "track_join_data.h"

namespace phase3
{

JoinedRelation rel_R;
JoinedRelation rel_S;

KeyCostMap key_cost_map;

int broadcast_cost(Node& node,
		std::vector<std::pair<Address, int>> &r,
		std::vector<std::pair<Address, int>> &s)
{
	int r_all = 0, r_local = 0, r_nodes = 0, s_nodes = 0;

	// size of key_node_service tuples
	// TODO use real value
	int tuple_size = 1;

	for (auto &elem : r)
	{
		r_all += elem.second;

		if (std::find_if(s.begin(), s.end(),
					[&elem](const std::pair<Address, int>& element) {
						return element.first.id() == elem.first.id();
					}) != s.end())
		{
			r_local += 1;
		}

		if (elem.first.id() != node.id())
			r_nodes += 1;
	}

	s_nodes = s.size();

	return r_all * s_nodes - r_local + r_nodes * s_nodes * tuple_size;
}

void master(Node& node, KeyCostMap& kc)
{
	for(auto it = kc.begin(), end = kc.end();
			it != end;
			it = kc.upper_bound(it->first)
	   ) {
		std::vector<std::pair<Address, int> > cost_R;
		std::vector<std::pair<Address, int> > cost_S;

		for (auto &tuple : it->second)
		{
			if (std::get<2>(tuple) == R)
			{
				cost_R.emplace_back(node.get_address(std::get<0>(tuple)), std::get<1>(tuple)/*multiply width R*/);
			}
			else
			{
				cost_S.emplace_back(node.get_address(std::get<0>(tuple)), std::get<1>(tuple)/*multiply width S*/);
			}
		}

		if (phase3::broadcast_cost(node, cost_R, cost_S) < phase3::broadcast_cost(node, cost_S, cost_R))
		{
			for (auto &tuple : it->second)
			{
				if (std::get<2>(tuple) == R)
				{
					for (auto &t : it->second)
					{
						if (std::get<2>(t) == S)
						{
							KeyNodeService key_node_service(it->first, node.get_address(std::get<0>(t)).id(), S);
							TCP_Client<KeyNodeService> client(key_node_service, node.get_address(std::get<0>(tuple)).ip(), node.get_address(std::get<0>(tuple)).service());
						}
					}
				}
			}
		}
		else
		{
			for (auto &tuple : it->second)
			{
				if (std::get<2>(tuple) == S)
				{
					for (auto &t : it->second)
					{
						if (std::get<2>(t) == R)
						{
							KeyNodeService key_node_service(it->first, node.get_address(std::get<0>(t)).id(), R);
							TCP_Client<KeyNodeService> client(key_node_service, node.get_address(std::get<0>(tuple)).ip(), node.get_address(std::get<0>(tuple)).service());
						}
					}
				}
			}
		}
	}
}

std::vector<TCP_Server<KeyNodeService>*> tcp_server_1;

void slave_1(Node& node, relation_type type, Relation& rel)
{
	// TODO port
	TCP_Server t = TCP_Server<KeyNodeService>(node.port() + type + 1, [&node, &rel](boost::shared_ptr<KeyNodeService> p, ConnectionPtr c) {
		RelationVector to_send;
		typedef std::multimap<int, std::string>::iterator MMAPIterator;
		std::pair<MMAPIterator, MMAPIterator> range = rel.equal_range(std::get<0>(*p));

		for (MMAPIterator it = range.first; it != range.second; it++)
		{
			to_send.emplace_back(it->first, it->second);
		}

		TCP_Client<RelationVector> client(to_send, node.get_address(std::get<1>(*p)).ip(), node.get_address(std::get<2>(*p)).id());
	});

	tcp_server_1.push_back(&t);
	tcp_server_1[type]->start();
}

std::vector<TCP_Server<RelationVector>*> tcp_server_2;

void slave_2(Node& node, relation_type type, Relation& rel, JoinedRelation& j_rel)
{
	// TODO port
	TCP_Server t = TCP_Server<RelationVector>(node.port() + type + 3, [&node, &type, &rel, &j_rel](boost::shared_ptr<RelationVector> p, ConnectionPtr c) {
		// for each vector sent by slave
		typedef std::multimap<int, std::string>::iterator MMAPIterator;
		std::pair<MMAPIterator, MMAPIterator> range = rel.equal_range((*p)[0].first);
		for (auto &elem : *p)
		{
			for (MMAPIterator it = range.first; it != range.second; it++)
			{
				if (type == R) j_rel.emplace_back(elem.first, elem.second, it->second);
				else j_rel.emplace_back(elem.first, it->second,  elem.second);
			}
		}
		// todo commit
	});

	tcp_server_2.push_back(&t);
	tcp_server_2[type]->start();
}

}

class Phase_3 : public Phase
{

public:

	Phase_3(Node& node) :
		Phase(node)
	{
		/* void */
	}

	virtual void execute(TJD& data)
	{
		std::cout << "Processing: Phase 3" << std::endl;

		boost::thread process_t {
			boost::bind(&phase3::master, boost::ref(node_), boost::ref(phase3::key_cost_map))
		};

		boost::thread process_r_1 {
			boost::bind(&phase3::slave_1, boost::ref(node_), R, boost::ref(data.rel_R))
		};

		boost::thread process_s_1 {
			boost::bind(&phase3::slave_1, boost::ref(node_), S, boost::ref(data.rel_S))
		};

		boost::thread process_r_2 {
			boost::bind(&phase3::slave_2, boost::ref(node_), R, boost::ref(data.rel_R), boost::ref(phase3::rel_R))
		};

		boost::thread process_s_2 {
			boost::bind(&phase3::slave_2, boost::ref(node_), S, boost::ref(data.rel_S), boost::ref(phase3::rel_S))
		};

		process_t.join();

		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate(TJD& data)
	{
		for (auto s : phase3::tcp_server_1)
			s->stop();

		for (auto s : phase3::tcp_server_2)
			s->stop();

		// TODO remove
		sleep(1);

		// TODO remove
		for (auto e : phase3::rel_R)
		{
			std::cout << std::get<0>(e) << ","
				<< std::get<1>(e) << ","
				<< std::get<2>(e) << std::endl;
		}
		for (auto e : phase3::rel_S)
		{
			std::cout << std::get<0>(e) << ","
				<< std::get<1>(e) << ","
				<< std::get<2>(e) << std::endl;
		}
	}

};

#endif  // SADDB_PHASE_3_H_
