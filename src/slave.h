#ifndef SADDB_SLAVE_H_
#define SADDB_SLAVE_H_

#include "node.h"
#include "relation.h"
#include "tcp_client.h"
#include "tcp_server.h"
#include "tcp_traits.h"
#include "track_join_data.h"

class Slave
{

public:

	Node node;
	Relation::Type type;
	Relation relation;
	Relation received;

	TCP_Server<SendCommand>* tcp_server_3_1;
	TCP_Server<Relation>* tcp_server_3_2;

public:

	Slave(Node& n, Relation::Type t);
	void phase_1();
	void phase_2();
	void phase_3_1();
	void phase_3_2();

};

#endif  // SADDB_SLAVE_H_
