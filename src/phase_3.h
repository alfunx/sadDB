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

std::vector<TCP_Server<SendCommand>*> tcp_server_1;
std::vector<TCP_Server<Relation>*> tcp_server_2;

int broadcast_cost(const Node& node,
		const std::vector<std::pair<Address, int>> &r,
		const std::vector<std::pair<Address, int>> &s)
{
	int r_all = 0, r_local = 0, r_nodes = 0, s_nodes = 0;

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

void broadcast(const Node& node, const KeyCostMap::iterator it, const relation_type type)
{
	for (auto &src : it->second)
	{
		if (std::get<2>(src) != type)
			continue;

		for (auto &dst : it->second)
		{
			if (std::get<2>(dst) == type)
				continue;

			SendCommand command(it->first,
					std::get<1>(dst),
					other_relation_type(type));

			std::cout << "master: to:" << std::get<1>(src)
				<< " type:" << std::get<2>(src)
				<< " port:" << node.get_address(std::get<1>(src)).port() + type + 2
				<< std::endl;

			TCP_Client<SendCommand> tcp_client(command,
					node.get_address(std::get<1>(src)).ip(),
					node.get_address(std::get<1>(src)).port() + type + 2);
			tcp_client.start();
		}
	}
}

void master(Node& node, KeyCostMap& kc)
{
	// TODO remove
	sleep(1);

	for(auto it = kc.begin(); it != kc.end(); ++it)
	{
		std::vector<std::pair<Address, int>> cost_R;
		std::vector<std::pair<Address, int>> cost_S;

		for (auto &tuple : it->second)
		{
			if (std::get<2>(tuple) == R)
			{
				// TODO multiply width R
				cost_R.emplace_back(node.get_address(std::get<1>(tuple)),
						std::get<3>(tuple));
			}
			else
			{
				// TODO multiply width S
				cost_S.emplace_back(node.get_address(std::get<1>(tuple)),
						std::get<3>(tuple));
			}
		}

		if (broadcast_cost(node, cost_R, cost_S) <
				broadcast_cost(node, cost_S, cost_R))
		{
			std::cout << "master: send R to S" << std::endl;
			broadcast(node, it, R);
		}
		else
		{
			std::cout << "master: send S to R" << std::endl;
			broadcast(node, it, S);
		}
	}
}

void slave_1(Node& node, relation_type type, RelationMap& rel)
{
	std::cout << "slave 1: type:" << type << " port:" << node.port() + type + 2 << std::endl;
	TCP_Server t = TCP_Server<SendCommand>(node.port() + type + 2,
			[&node, &type, &rel](boost::shared_ptr<SendCommand> p, ConnectionPtr c) {
		Relation to_send;
		typedef std::multimap<int, std::string>::iterator MultiMapIt;
		std::pair<MultiMapIt, MultiMapIt> range = rel.equal_range(std::get<0>(*p));

		for (MultiMapIt it = range.first; it != range.second; ++it)
		{
			to_send.emplace_back(it->first, it->second);
		}

		std::cout << "slave 1: to:" << std::get<1>(*p)
			<< " me:" << type
			<< " type:" << std::get<2>(*p)
			<< " port:" << node.get_address(std::get<1>(*p)).port() + std::get<2>(*p) + 4
			<< std::endl;

		TCP_Client<Relation> tcp_client(to_send,
				node.get_address(std::get<1>(*p)).ip(),
				node.get_address(std::get<1>(*p)).port() + std::get<2>(*p) + 4);
		tcp_client.start();
	});

	tcp_server_1.push_back(&t);
	t.start();
}

void slave_2(Node& node, relation_type type, RelationMap& rel, JoinedRelation& j_rel)
{
	std::cout << "slave 2: type:" << type << " port:" << node.port() + type + 4 << std::endl;
	TCP_Server t = TCP_Server<Relation>(node.port() + type + 4,
			[&node, &type, &rel, &j_rel](boost::shared_ptr<Relation> p, ConnectionPtr c) {
		// for each vector sent by slave
		typedef std::multimap<int, std::string>::iterator MultiMapIt;
		std::pair<MultiMapIt, MultiMapIt> range = rel.equal_range((*p)[0].first);

		for (auto &elem : *p)
		{
			for (MultiMapIt it = range.first; it != range.second; ++it)
			{
				if (type == R)
					j_rel.emplace_back(elem.first, elem.second, it->second);
				else
					j_rel.emplace_back(elem.first, it->second,  elem.second);
			}
		}

		// TODO commit
	});

	tcp_server_2.push_back(&t);
	t.start();
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

		boost::thread process_r_2 {
			boost::bind(&phase3::slave_2, boost::ref(node_), R, boost::ref(data.rel_R), boost::ref(phase3::rel_R))
		};

		boost::thread process_s_2 {
			boost::bind(&phase3::slave_2, boost::ref(node_), S, boost::ref(data.rel_S), boost::ref(phase3::rel_S))
		};

		boost::thread process_r_1 {
			boost::bind(&phase3::slave_1, boost::ref(node_), R, boost::ref(data.rel_R))
		};

		boost::thread process_s_1 {
			boost::bind(&phase3::slave_1, boost::ref(node_), S, boost::ref(data.rel_S))
		};

		boost::thread process_t {
			boost::bind(&phase3::master, boost::ref(node_), boost::ref(data.key_cost_map))
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
		std::cout << "- result: " << std::endl;
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
