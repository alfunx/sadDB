#ifndef SADDB_MASTER_H
#define SADDB_MASTER_H

#include <map>
#include <vector>

#include "node.h"
#include "address.h"
#include "enums.h"


class Master
{
  std::map<int, std::vector<std::tuple<Address, int, relation_type> > > key_node_cost_type;
	Node node_;

public:

	Master(Node node);

	void phase2();

	void phase3();

	int broadcast(std::vector<std::pair<Address, int> > &r, std::vector<std::pair<Address, int> > &s);

};

#endif //SADDB_MASTER_H
