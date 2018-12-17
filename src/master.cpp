#include "master.h"
#include "measurement_traits.h"

Master::Master(Node& n) :
	node(n)
{
	/* void */
}

void Master::phase_2()
{
	KeyCostMap kcm;
	TCP_Server<KeyCost> tcp_s = TCP_Server<KeyCost>(node.port() + 1,
			[&kcm](boost::shared_ptr<KeyCost> p, ConnectionPtr c) {
		kcm[p->key].push_back(*p);
	});
	tcp_server_2 = &tcp_s;
	tcp_server_2->start();

	key_cost_map = kcm;
}

void Master::phase_3()
{
	for(auto it = key_cost_map.begin(); it != key_cost_map.end(); ++it)
	{
		std::vector<std::pair<Address, int>> cost_R;
		std::vector<std::pair<Address, int>> cost_S;

		for (auto &tuple : it->second)
		{
			if (tuple.type == Relation::Type::R)
			{
				cost_R.emplace_back(node.get_address(tuple.id),
						tuple.cost);
			}
			else
			{
				cost_S.emplace_back(node.get_address(tuple.id),
						tuple.cost);
			}
		}

		if (broadcast_cost(cost_R, cost_S) <
				broadcast_cost(cost_S, cost_R))
		{
			broadcast(it, Relation::Type::R);
		}
		else
		{
			broadcast(it, Relation::Type::S);
		}
	}
}

unsigned int Master::broadcast_cost(const AddressCostList &r, const AddressCostList &s)
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

void Master::broadcast(const KeyCostMap::iterator it, const Relation::Type t)
{
	for (auto &src : it->second)
	{
		if (src.type != t)
			continue;

		for (auto &dst : it->second)
		{
			if (dst.type == t)
				continue;

			SendCommand command(it->first, dst.id);

			TCP_Client<SendCommand> tcp_c(command,
					node.get_address(src.id).ip(),
					node.get_address(src.id).port() + t + 2);
			tcp_c.start();

			if (node.id() != src.id)
			{
				++phase_3_sent_messages_count;
				phase_3_sent_messages_sizes.push_back(measurement_traits::size(command));
			}
		}
	}
}
