#include <fstream>
#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>

#include "slave.h"
#include "tcp_client.h"

// Only for testing
void print(std::multimap<int, std::string> &m)
{
    std::multimap<int, std::string>::iterator it1 = m.begin();
    std::multimap<int, std::string>::iterator it2 = m.end();

    for(; it1!=it2; ++it1)
    {
        std::cout<<"key = "<<(it1->first)<<"\t\t";
        std::cout<<"value = "<<(it1->second)<<std::endl;
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

void Slave::phase1() {
    // Fill a multimap with relation data (either R or S)
    std::string r_file = "../relations/";
    r_file.append(experiment);
    r_file.append("/").append(node_nr);

    // We should fill two strings after run program with
    // the correct path and name for the chosen experiment
    // and also node number
    switch (type)
    {
        case R : r_file.append("/R.txt");
            break;
        case S : r_file.append("/S.txt");
            break;
        default:
            break;
    }
    std::ifstream ifs(r_file);
    boost::archive::text_iarchive serial2(ifs);
    serial2 >> relation;

    // todo remove
    print(relation);
}

void Slave::phase2() {
    // Go through each distinct key
    for( auto it = relation.begin(), end = relation.end();
         it != end;
         it = relation.upper_bound(it->first)
            ) {
        std::pair<int, int> x(it->first, relation.count(it->first));
        // Send x (key, count) to processT (hash(k) mod N)

        // Only for testing
        std::cout << type << " -- " << x.first << ": " << x.second << std::endl;
    }
}

void Slave::phase3() {
	// todo listen for tuples from master and slave separately
//	std::tuple<int, int, relation_type> key_node_service(/*key*/1, /*id*/1, /*relation_type*/S);
//	// for each tuple if sent by master
//	std::vector<std::pair<int, std::string>> to_send;
//	typedef std::multimap<int, std::string>::iterator MMAPIterator;
//	std::pair<MMAPIterator, MMAPIterator> range = relation.equal_range(std::get<0>(key_node_service));
//
//	for (MMAPIterator it = range.first; it != range.second; it++)
//	{
//		to_send.emplace_back(it->first, it->second);
//	}
//	TCP_Client<std::vector<std::pair<int, std::string>>> client(to_send, node_.get_ip(std::get<1>(key_node_service)), node_.get_port(std::get<2>(key_node_service)));
//


}

void Slave::handle_sent_by_slave(std::vector<std::pair<int, std::string>> received) {
	// for each vector sent by slave
	std::vector<std::tuple<int, std::string, std::string>> to_send;
	typedef std::multimap<int, std::string>::iterator MMAPIterator;
	std::pair<MMAPIterator, MMAPIterator> range = relation.equal_range(received[0].first);
	for (auto &elem : received)
	{
		for (MMAPIterator it = range.first; it != range.second; it++)
		{
			if (type == R) to_send.emplace_back(elem.first, elem.second, it->second);
			else to_send.emplace_back(elem.first, it->second,  elem.second);
		}
	}
	// todo commit
}

void Slave::handle_sent_by_master(std::tuple<int, int, relation_type> key_node_service) {
	std::vector<std::pair<int, std::string>> to_send;
	typedef std::multimap<int, std::string>::iterator MMAPIterator;
	std::pair<MMAPIterator, MMAPIterator> range = relation.equal_range(std::get<0>(key_node_service));

	for (MMAPIterator it = range.first; it != range.second; it++)
	{
		to_send.emplace_back(it->first, it->second);
	}
	TCP_Client<std::vector<std::pair<int, std::string>>> client(to_send, node_.get_ip(std::get<1>(key_node_service)), node_.get_port(std::get<2>(key_node_service)));

}
