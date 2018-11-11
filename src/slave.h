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

public:

	Slave(Node node, relation_type t, std::string experiment, std::string node_nr);

	void phase1();

	void phase2();

	void phase3();

};

#endif //SADDB_SLAVE_H
