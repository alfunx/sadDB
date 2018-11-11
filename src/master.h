#ifndef SADDB_MASTER_H
#define SADDB_MASTER_H

#include <map>

#include "node.h"

class Master
{

	Node node_;

	// Here we need a multimap Tr|s
	// with a <int, struct tobedefined>

	int broadcast_r_s();

	int broadcast_s_r();

public:

	Master(Node node);

	void phase2();

	void phase3();

};

#endif //SADDB_MASTER_H
