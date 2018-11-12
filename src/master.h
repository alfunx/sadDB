#ifndef SADDB_MASTER_H
#define SADDB_MASTER_H

#include <map>
#include <vector>

#include "address.h"
#include "enums.h"
#include "node.h"
#include "tcp_server.h"

typedef std::tuple<int, unsigned int, int> KeyCost;

class Master
{

public:

	std::map<int, std::vector<std::tuple<Address, int, relation_type> > > key_node_cost_type;
	std::vector<KeyCost> key_cost;
	Node node_;

public:

	TCP_Server<KeyCost>* tcp_server;

	Master(Node node);

	void phase2();

	void phase3();

	int broadcast(std::vector<std::pair<Address, int> > &r, std::vector<std::pair<Address, int> > &s);

};

#endif //SADDB_MASTER_H
