#include <algorithm>

#include "master.h"
#include "serialize_tuple.h"
#include "tcp_client.h"

int Master::broadcast(std::vector<std::pair<Address, int> > &r, std::vector<std::pair<Address, int> > &s) {
    int r_all = 0, r_local = 0, r_nodes = 0, s_nodes = 0;
    // m is size of key_node_service pairs below!
    // TODO add the real value!
    int m = 1;

    for (auto &elem : r)
    {
        r_all += elem.second;
        if (std::find_if(s.begin(), s.end(), [&elem](const std::pair<Address, int>& element)
        {return element.first.id() == elem.first.id();}) != s.end())
		{
        	r_local += 1;
		}
        if (elem.first.id() != node_.id()) r_nodes += 1;
    }
    s_nodes = s.size();

    return r_all * s_nodes - r_local + r_nodes * s_nodes * m;
}

Master::Master(Node node) : node_(node)
{
	/* void */
}

void Master::phase2()
{
	// wait for incoming broadcast
	std::vector<KeyCost> kc;
	TCP_Server<KeyCost> t(node_.port() + 1, [&kc](boost::shared_ptr<KeyCost> p, ConnectionPtr c) {
		kc.push_back(*p);
	});
	tcp_server = &t;
	tcp_server->start();

	key_cost = kc;

	// TODO remove
	for (auto x : kc)
	{
		std::cout << std::get<0>(x) << ","
			<< std::get<1>(x) << ","
			<< std::get<2>(x) << std::endl;
	}
}

void Master::phase3() {

    for( auto it = key_node_cost_type.begin(), end = key_node_cost_type.end();
         it != end;
         it = key_node_cost_type.upper_bound(it->first)
            ) {
        std::vector<std::pair<Address, int> > cost_R;
        std::vector<std::pair<Address, int> > cost_S;
        for (auto &tuple : it->second)
        {
            if (std::get<2>(tuple) == R)
            {
                cost_R.emplace_back(std::get<0>(tuple), std::get<1>(tuple)/*multiply width R*/);
            }
            else
            {
                cost_S.emplace_back(std::get<0>(tuple), std::get<1>(tuple)/*multiply width S*/);
            }
        }
        if (broadcast(cost_R, cost_S) < broadcast(cost_S, cost_R))
        {
            for (auto &tuple : it->second)
            {
                if (std::get<2>(tuple) == R)
                {
                    for (auto &t : it->second)
                    {
                        if (std::get<2>(t) == S)
                        {
                            std::tuple<int, int, relation_type> key_node_service(it->first, std::get<0>(t).id(), S);
                            TCP_Client<std::tuple<int, int, relation_type>> client(key_node_service, std::get<0>(tuple).ip(), std::get<0>(tuple).service());
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
                            std::tuple<int, int, relation_type> key_node_service(it->first, std::get<0>(t).id(), R);
                            TCP_Client<std::tuple<int, int, relation_type>> client(key_node_service, std::get<0>(tuple).ip(), std::get<0>(tuple).service());
                        }
                    }
                }
            }
        }
    }
}

