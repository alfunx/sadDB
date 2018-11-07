#ifndef SADDB_MASTER_H
#define SADDB_MASTER_H

#include <map>

class Master
{
    // Here we need a multimap Tr|s
    // with a <int, struct tobedefined>

    int broadcast_r_s();

    int broadcast_s_r();

public:
    void phase2();

    void phase3();
};

#endif //SADDB_MASTER_H
