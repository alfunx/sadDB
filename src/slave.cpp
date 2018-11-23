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

		// TODO remove
		std::cerr << "- " << cost
			<< " - to: " << address << std::endl;

		TCP_Client<KeyCost> tcp_c(cost, address.ip(), address.port() + 1);
		tcp_c.start();
	}
}

void Slave::phase_3_1()
{
	auto rel = relation;
	auto n = node;
	auto t = type;

	TCP_Server tcp_s = TCP_Server<SendCommand>(node.port() + type + 2,
			[&rel, &n, &t](boost::shared_ptr<SendCommand> p, ConnectionPtr c) {
		Relation sel = Relation::select(rel, p->key);

		std::cerr << "slave 1: to:" << p->id
			<< " type:" << t
			<< " port:" << n.get_address(p->id).port() + Relation::other(t) + 4
			<< std::endl;

		TCP_Client<Relation> tcp_c(sel,
				n.get_address(p->id).ip(),
				n.get_address(p->id).port() + Relation::other(t) + 4);
		tcp_c.start();
	});

	tcp_server_3_1 = &tcp_s;
	tcp_s.start();
}

void Slave::phase_3_2()
{
	Relation rec;

	TCP_Server tcp_s = TCP_Server<Relation>(node.port() + type + 4,
			[&rec](boost::shared_ptr<Relation> p, ConnectionPtr c) {
		rec.union_all(*p);
	});

	tcp_server_3_2 = &tcp_s;
	tcp_s.start();

	received = rec;
}
