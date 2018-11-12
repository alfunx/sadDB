#ifndef SADDB_SLAVE_H
#define SADDB_SLAVE_H

#include <map>

#include "node.h"
#include "enums.h"

class Slave
{

	Node node_;
	std::string experiment;
	std::string node_nr;
	relation_type type;
	std::multimap<int, std::string> relation;

	void handle_sent_by_slave(std::vector<std::pair<int, std::string>> received);

	void handle_sent_by_master(std::tuple<int, int, relation_type> key_node_service);

public:

	Slave(Node node, relation_type t, std::string experiment, std::string node_nr);

	void phase1();

	void phase2();

	void phase3();

};

#endif //SADDB_SLAVE_H
