#include <fstream>
#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>

#include "serialize_tuple.h"
#include "slave.h"
#include "tcp_client.h"

// TODO remove
void print(Relation& relation)
{
	for(auto t : relation)
	{
		std::cout << "key = " << t.first << "\t\t"
			<< "value = " << t.second << std::endl;
	}
	std::cout<<std::endl;
}

Slave::Slave(Node node, relation_type t, std::string e, std::string n) :
	node_(node),
	type(t),
	experiment(e),
	node_nr(n)
{
	/* void */
}

void Slave::phase1()
{
	// read relations
	std::string r_file = std::string("../relations/")
		.append(experiment)
		.append("/")
		.append(std::to_string(node_.id()))
		.append("/");

	// We should fill two strings after run program with the correct path
	// and name for the chosen experiment and also node number.
	switch (type)
	{
		case R:
			r_file.append("R.txt");
			break;
		case S:
			r_file.append("S.txt");
			break;
		default:
			break;
	}

	std::ifstream ifs(r_file);
	boost::archive::text_iarchive serial(ifs);
	serial >> relation;

	// TODO remove
	print(relation);
}

void Slave::phase2()
{
	// iterate over distinct key
	for(auto it = relation.begin(), end = relation.end();
			it != end;
			it = relation.upper_bound(it->first)) {
		KeyCost t(it->first, node_.id(), relation.count(it->first));

		// TODO send x (key, count) to processT (hash(k) mod N)
		unsigned int n = std::get<0>(t) % node_.servers()->size() + 1;
		Address address;

		for (auto a : *node_.servers())
		{
			if (a.id() == n)
			{
				address = a;
				break;
			}
		}

		// TODO remove
		std::cout << type << " -- "
			<< std::get<0>(t) << ":"
			<< std::get<2>(t) << ", "
			<< "to: " << address << std::endl;

		TCP_Client<KeyCost> tcp_client(t, address.ip(), address.port() + 1);
		tcp_client.start();
	}
}

void Slave::phase3()
{

}
