#include "master.h"


int broadcast_R_S() {
    // TODO implement
    return 2;
}

int broadcast_S_R() {
    // TODO implement
    return 1;

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
        if (broadcast_R_S() < broadcast_S_R())
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
                            // TODO send <k, nS> to nR (get<0>(t) -> maybe get address!)
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
                            // TODO send <k, nR> to nS (get<0>(t) -> maybe get address!)
                        }
                    }
                }
            }
        }
    }
}

