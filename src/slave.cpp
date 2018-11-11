#include <fstream>
#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>

#include "slave.h"

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


Slave::Slave(Node node, relation_type t, std::string e) :
	node_(node),
	type(t),
	experiment(e)
{
	/* void */
}

void Slave::phase1() {
    // Fill a multimap with relation data (either R or S)
    std::string r_file = "../relations/";
    r_file.append(experiment);
    r_file.append("/").append(std::to_string(node_.id()));

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

}
