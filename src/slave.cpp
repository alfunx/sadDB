#include <fstream>

#include "slave.h"

Slave::Slave(Node& n, Relation::Type t) :
	node(n),
	type(t)
{
	/* void */
}

void Slave::phase_1()
{
	// relations directory
	std::string rel_dir = std::string("../relations/")
		.append(node.database())
		.append("/")
		.append(std::to_string(node.id()))
		.append("/");

	if (type == Relation::Type::R)
		rel_dir.append("R.txt");
	else if (type == Relation::Type::S)
		rel_dir.append("S.txt");

	std::ifstream ifs(rel_dir);
	boost::archive::text_iarchive serial(ifs);
	serial >> relation;
}

void Slave::phase_2()
{
	auto keys = relation.distinct_keys();

	// iterate over distinct key
	for (auto key : keys)
	{
		KeyCost cost(key, node.id(), type, relation.count_keys(key));

		// TODO send x (key, count) to processT (hash(k) mod N)
		unsigned int id = key % node.servers()->size() + 1;
		Address address = node.get_address(id);

		TCP_Client<KeyCost> tcp_c(cost, address.ip(), address.port() + 1);
		tcp_c.start();
		++sent_messages_count;
	}
}

void Slave::phase_3_1()
{
	TCP_Server tcp_s = TCP_Server<SendCommand>(node.port() + type + 2,
			[this](boost::shared_ptr<SendCommand> p, ConnectionPtr c) {
		Relation sel = Relation::select(relation, p->key);

		TCP_Client<Relation> tcp_c(sel,
				node.get_address(p->id).ip(),
				node.get_address(p->id).port() + Relation::other(type) + 4);
		tcp_c.start();
		++sent_messages_count;
	});

	tcp_server_3_1 = &tcp_s;
	tcp_s.start();
}

void Slave::phase_3_2()
{
	TCP_Server tcp_s = TCP_Server<Relation>(node.port() + type + 4,
			[this](boost::shared_ptr<Relation> p, ConnectionPtr c) {
		received.union_all(*p);
	});

	tcp_server_3_2 = &tcp_s;
	tcp_s.start();
}

int Slave::sent_messages()
{
	return sent_messages_count;
}
