#ifndef SADDB_PHASE_1_H_
#define SADDB_PHASE_1_H_

#include <iostream>

#include "phase.h"

namespace phase1
{

class Phase_1 : public Phase
{

boost::thread process_r;
boost::thread process_s;

public:

	Phase_1(Node& node, Master& master, Slave& slave_r, Slave& slave_s) :
		Phase(node, master, slave_r, slave_s)
	{
		/* void */
	}

	virtual void execute()
	{
		std::cout << "Processing: Phase 1" << std::endl;

		process_r = boost::thread {
			boost::bind(&Slave::phase_1,
					&slave_r)
		};

		process_s = boost::thread {
			boost::bind(&Slave::phase_1,
					&slave_s)
		};

		process_r.join();
		process_s.join();

		// TODO remove
		std::cout << "R:" << std::endl;
		std::cout << slave_r.relation << std::endl;
		std::cout << "S:" << std::endl;
		std::cout << slave_s.relation << std::endl;

		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate()
	{
		/* void */
	}

};

}

using phase1::Phase_1;

#endif  // SADDB_PHASE_1_H_
