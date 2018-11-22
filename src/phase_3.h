#ifndef SADDB_PHASE_3_H_
#define SADDB_PHASE_3_H_

#include <iostream>

#include "node.h"
#include "phase.h"
#include "relation.h"
#include "serialize_tuple.h"
#include "tcp_traits.h"
#include "track_join_data.h"

namespace phase3
{

boost::thread process_r_2;
boost::thread process_s_2;
boost::thread process_r_1;
boost::thread process_s_1;
boost::thread process_t;

Relation received_rel_R;
Relation received_rel_S;

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

void broadcast(const Node& node, const KeyCostMap::iterator it, const Relation::Type type)
{
	for (auto &src : it->second)
	{
		if (src.type != type)
			continue;

		for (auto &dst : it->second)
		{
			if (dst.type == type)
				continue;

			SendCommand command(it->first, dst.id);

			std::cout << "master: to:" << src.id
				<< " type:" << src.type
				<< " port:" << node.get_address(src.id).port() + type + 2
				<< std::endl;

			TCP_Client<SendCommand> tcp_client(command,
					node.get_address(src.id).ip(),
					node.get_address(src.id).port() + type + 2);
			tcp_client.start();
		}
	}
}

void master(Node& node, KeyCostMap& kc)
{
	for(auto it = kc.begin(); it != kc.end(); ++it)
	{
		std::vector<std::pair<Address, int>> cost_R;
		std::vector<std::pair<Address, int>> cost_S;

		for (auto &tuple : it->second)
		{
			if (tuple.type == Relation::Type::R)
			{
				// TODO multiply width R
				cost_R.emplace_back(node.get_address(tuple.id),
						tuple.cost);
			}
			else
			{
				// TODO multiply width S
				cost_S.emplace_back(node.get_address(tuple.id),
						tuple.cost);
			}
		}

		if (broadcast_cost(node, cost_R, cost_S) <
				broadcast_cost(node, cost_S, cost_R))
		{
			std::cout << "master: send R to S" << std::endl;
			broadcast(node, it, Relation::Type::R);
		}
		else
		{
			std::cout << "master: send S to R" << std::endl;
			broadcast(node, it, Relation::Type::S);
		}
	}
}

void slave_1(Node& node, Relation::Type type, Relation& rel)
{
	std::cout << "slave 1: type:" << type << " port:" << node.port() + type + 2 << std::endl;
	TCP_Server t = TCP_Server<SendCommand>(node.port() + type + 2,
			[&node, &type, &rel](boost::shared_ptr<SendCommand> p, ConnectionPtr c) {
		Relation sel = rel.copy();
		sel.select(p->key);

		std::cout << "slave 1: to:" << p->id
			<< " type:" << type
			<< " port:" << node.get_address(p->id).port() + Relation::other(type) + 4
			<< std::endl;

		TCP_Client<Relation> tcp_client(sel,
				node.get_address(p->id).ip(),
				node.get_address(p->id).port() + Relation::other(type) + 4);
		tcp_client.start();
	});

	tcp_server_1.push_back(&t);
	t.start();
}

void slave_2(Node& node, Relation::Type type, Relation& rel, Relation& received)
{
	std::cout << "slave 2: type:" << type << " port:" << node.port() + type + 4 << std::endl;
	TCP_Server t = TCP_Server<Relation>(node.port() + type + 4,
			[&node, &type, &rel, &received](boost::shared_ptr<Relation> p, ConnectionPtr c) {
		received.union_all(*p);
	});

	tcp_server_2.push_back(&t);
	t.start();
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

		process_r_2 = boost::thread {
			boost::bind(&slave_2,
					boost::ref(node_),
					Relation::Type::R,
					boost::ref(data.rel_R),
					boost::ref(received_rel_S))
		};

		process_s_2 = boost::thread {
			boost::bind(&slave_2,
					boost::ref(node_),
					Relation::Type::S,
					boost::ref(data.rel_S),
					boost::ref(received_rel_R))
		};

		process_r_1 = boost::thread {
			boost::bind(&slave_1,
					boost::ref(node_),
					Relation::Type::R,
					boost::ref(data.rel_R))
		};

		process_s_1 = boost::thread {
			boost::bind(&slave_1,
					boost::ref(node_),
					Relation::Type::S,
					boost::ref(data.rel_S))
		};

		process_t = boost::thread {
			boost::bind(&master,
					boost::ref(node_),
					boost::ref(data.key_cost_map))
		};

		// wait for thread
		process_t.join();

		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate(TJD& data)
	{
		for (auto s : tcp_server_1)
			s->stop();

		for (auto s : tcp_server_2)
			s->stop();

		// wait for threads
		process_r_2.join();
		process_s_2.join();
		process_r_1.join();
		process_s_1.join();

		auto result = data.rel_R.copy();
		result.join(received_rel_S);
		auto tmp = received_rel_R.copy();
		tmp.join(data.rel_S);
		result.union_all(tmp);

		// TODO remove
		std::cout << "- result: " << std::endl;
		std::cout << result << std::endl;
	}

};

}

using phase3::Phase_3;

#endif  // SADDB_PHASE_3_H_
