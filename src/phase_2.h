#ifndef SADDB_PHASE_2_H_
#define SADDB_PHASE_2_H_

#include <iostream>

#include "phase.h"

namespace phase2
{

boost::thread process_t;
boost::thread process_r;
boost::thread process_s;

class Phase_2 : public Phase
{

public:

	Phase_2(Node& node, Master& master, Slave& slave_r, Slave& slave_s) :
		Phase(node, master, slave_r, slave_s)
	{
		/* void */
	}

	virtual void execute()
	{
		std::cout << "Processing: Phase 2" << std::endl;

		process_t = boost::thread {
			boost::bind(&Master::phase_2,
					&master)
		};

		process_r = boost::thread {
			boost::bind(&Slave::phase_2,
					&slave_r)
		};

		process_s = boost::thread {
			boost::bind(&Slave::phase_2,
					&slave_s)
		};

		// wait for threads
		process_r.join();
		process_s.join();

		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate()
	{
		master.tcp_server_2->stop();

		// wait for thread
		process_t.join();
	}

};

}

using phase2::Phase_2;

#endif  // SADDB_PHASE_2_H_
