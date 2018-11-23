#ifndef SADDB_PHASE_H_
#define SADDB_PHASE_H_

#include <iostream>

#include "node.h"
#include "relation.h"
#include "tcp_traits.h"
#include "track_join_data.h"

#include "master.h"
#include "slave.h"

class Phase
{

protected:

	Node& node_;
	Master& master;
	Slave& slave_r;
	Slave& slave_s;

public:

	Phase(Node& node, Master& master, Slave& slave_r, Slave& slave_s) :
		node_(node),
		master(master),
		slave_r(slave_r),
		slave_s(slave_s)
	{
		/* void */
	}

	virtual void execute() =0;
	virtual void terminate() =0;

	void confirm()
	{
		tcp_traits::confirm(node_.client());
	}

};

class RandomPhase : public Phase
{

public:

	RandomPhase(Node& node, Master& master, Slave& slave_r, Slave& slave_s) :
		Phase(node, master, slave_r, slave_s)
	{
		/* void */
	}

	virtual void execute()
	{
		std::cerr << "Processing: Random Phase" << std::endl;
		sleep((rand() % 6) + 1);
		std::cerr << "Done." << std::endl;

		// confirm, await next command
		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate()
	{
		/* void */
	}

};

#endif  // SADDB_PHASE_H_
