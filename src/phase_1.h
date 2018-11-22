#ifndef SADDB_PHASE_1_H_
#define SADDB_PHASE_1_H_

#include <fstream>
#include <iostream>

#include "node.h"
#include "phase.h"
#include "relation.h"
#include "serialize_tuple.h"
#include "tcp_traits.h"
#include "track_join_data.h"

namespace phase1
{

void slave(Node& node, Relation::Type type, Relation& rel)
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
	serial >> rel;
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

		Relation rel_R;
		Relation rel_S;

		boost::thread process_r {
			boost::bind(&slave,
					boost::ref(node_),
					Relation::Type::R,
					boost::ref(rel_R))
		};

		boost::thread process_s {
			boost::bind(&slave,
					boost::ref(node_),
					Relation::Type::S,
					boost::ref(rel_S))
		};

		process_r.join();
		process_s.join();

		// set permanent
		data.rel_R = rel_R;
		data.rel_S = rel_S;

		// TODO remove
		std::cout << "R:" << std::endl;
		std::cout << data.rel_R << std::endl;
		std::cout << "S:" << std::endl;
		std::cout << data.rel_S << std::endl;

		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate(TJD& data)
	{
		/* void */
	}

};

}

using phase1::Phase_1;

#endif  // SADDB_PHASE_1_H_
