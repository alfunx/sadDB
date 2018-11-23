#ifndef SADDB_PHASE_3_H_
#define SADDB_PHASE_3_H_

#include <iostream>

#include "phase.h"

namespace phase3
{

boost::thread process_r_2;
boost::thread process_s_2;
boost::thread process_r_1;
boost::thread process_s_1;
boost::thread process_t;

class Phase_3 : public Phase
{

public:

	Phase_3(Node& node, Master& master, Slave& slave_r, Slave& slave_s) :
		Phase(node, master, slave_r, slave_s)
	{
		/* void */
	}

	virtual void execute()
	{
		std::cout << "Processing: Phase 3" << std::endl;

		process_r_2 = boost::thread {
			boost::bind(&Slave::phase_3_1,
					&slave_r)
		};

		process_s_2 = boost::thread {
			boost::bind(&Slave::phase_3_1,
					&slave_s)
		};

		process_r_1 = boost::thread {
			boost::bind(&Slave::phase_3_2,
					&slave_r)
		};

		process_s_1 = boost::thread {
			boost::bind(&Slave::phase_3_2,
					&slave_s)
		};

		process_t = boost::thread {
			boost::bind(&Master::phase_3,
					&master)
		};

		// wait for thread
		process_t.join();

		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate()
	{
		slave_r.tcp_server_3_1->stop();
		slave_s.tcp_server_3_1->stop();
		slave_r.tcp_server_3_2->stop();
		slave_s.tcp_server_3_2->stop();

		// wait for threads
		process_r_2.join();
		process_s_2.join();
		process_r_1.join();
		process_s_1.join();

		// perform join
		auto result = Relation::join(slave_r.relation, slave_r.received);
		auto tmp = Relation::join(slave_s.received, slave_s.relation);
		result.union_all(tmp);

		// print join results
		std::cout << "\n>>> result:\n"
			<< result << std::endl;
	}

};

}

using phase3::Phase_3;

#endif  // SADDB_PHASE_3_H_
