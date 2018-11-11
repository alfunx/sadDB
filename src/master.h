#ifndef SADDB_MASTER_H
#define SADDB_MASTER_H

#include <map>
#include <vector>

#include "address.h"

enum relation_type {R, S};

class Master
{
    std::map<int, std::vector<std::tuple<Address, int, relation_type> > > key_node_cost_type;

public:
    void phase2();

    void phase3();
};

#endif //SADDB_MASTER_H
