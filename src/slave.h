#ifndef SADDB_SLAVE_H
#define SADDB_SLAVE_H

#include <map>

enum relation_type {R, S};

class Slave
{
    std::string experiment;
    std::string node_nr;
    relation_type type;
    std::multimap<int, std::string> relation;

public:
    Slave(relation_type t, std::string experiment, std::string node_nr);

    void phase1();

    void phase2();

    void phase3();
};

#endif //SADDB_SLAVE_H
