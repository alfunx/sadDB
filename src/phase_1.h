#ifndef SADDB_PHASE_1_H_
#define SADDB_PHASE_1_H_

#include <fstream>
#include <iostream>

#include "enums.h"
#include "node.h"
#include "phase.h"
#include "serialize_tuple.h"
#include "tcp_traits.h"
#include "track_join_data.h"

namespace phase1
{

// TODO remove
void print(RelationMap& rel)
{
	for(auto t : rel)
	{
		std::cout << "key:" << std::get<0>(t)
			<< " - value:" << std::get<1>(t) << std::endl;
	}
	std::cout << std::endl;
}

void slave(Node& node, relation_type type, RelationMap& rel)
{
	// read relations
	std::string r_file = std::string("../relations/")
		.append(node.database())
		.append("/")
		.append(std::to_string(node.id()))
		.append("/");

	// We should fill two strings after run program with the correct path
	// and name for the chosen experiment and also node number.
	if (type == R)
		r_file.append("R.txt");
	else if (type == S)
		r_file.append("S.txt");

	std::ifstream ifs(r_file);
	boost::archive::text_iarchive serial(ifs);
	serial >> rel;
}

}

class Phase_1 : public Phase
{

public:

	Phase_1(Node& node) :
		Phase(node)
	{
		/* void */
	}

	virtual void execute(TJD& data)
	{
		std::cout << "Processing: Phase 1" << std::endl;

		RelationMap rel_R;
		RelationMap rel_S;

		boost::thread process_r {
			boost::bind(&phase1::slave, boost::ref(node_), R, boost::ref(rel_R))
		};

		boost::thread process_s {
			boost::bind(&phase1::slave, boost::ref(node_), S, boost::ref(rel_S))
		};

		process_r.join();
		process_s.join();

		// set permanent
		data.rel_R = rel_R;
		data.rel_S = rel_S;

		// TODO remove
		std::cout << "R:" << std::endl;
		phase1::print(data.rel_R);
		std::cout << "S:" << std::endl;
		phase1::print(data.rel_S);

		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate(TJD& data)
	{
		/* void */
	}

};

#endif  // SADDB_PHASE_1_H_
