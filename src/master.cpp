#include <algorithm>

#include "master.h"
#include "tcp_client.h"
#include "serialize_tuple.h"


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

void Master::phase2() {

}

void Master::phase3() {

    for( auto it = key_node_cost_type.begin(), end = key_node_cost_type.end();
         it != end;
         it = key_node_cost_type.upper_bound(it->first)
            ) {
        std::vector<std::pair<Address, int> > cost_R;
        std::vector<std::pair<Address, int> > cost_S;
        for (auto &tup : it->second)
        {
            if (std::get<2>(tup) == R)
            {
                cost_R.emplace_back(std::get<0>(tup), std::get<1>(tup)/*multiply width R*/);
            }
            else
            {
                cost_S.emplace_back(std::get<0>(tup), std::get<1>(tup)/*multiply width S*/);
            }
        }
        if (broadcast(cost_R, cost_S) < broadcast(cost_S, cost_R))
        {
            for (auto &tup : it->second)
            {
                if (std::get<2>(tup) == R)
                {
                    for (auto &t : it->second)
                    {
                        if (std::get<2>(t) == S)
                        {
                            std::tuple<int, int, relation_type> key_node_service(it->first, std::get<0>(t).id(), S);
                            TCP_Client<std::tuple<int, int, relation_type>> client(key_node_service, std::get<0>(tup).ip(), std::get<0>(tup).service());
                        }
                    }
                }
            }
        }
        else
        {
            for (auto &tup : it->second)
            {
                if (std::get<2>(tup) == S)
                {
                    for (auto &t : it->second)
                    {
                        if (std::get<2>(t) == R)
                        {
                            std::tuple<int, int, relation_type> key_node_service(it->first, std::get<0>(t).id(), R);
                            TCP_Client<std::tuple<int, int, relation_type>> client(key_node_service, std::get<0>(tup).ip(), std::get<0>(tup).service());
                        }
                    }
                }
            }
        }
    }
}

