#ifndef SADDB_PHASE_H_
#define SADDB_PHASE_H_

#include <iostream>

#include "node.h"
#include "master.h"
#include "slave.h"
#include "tcp_traits.h"

class Phase
{

protected:

	Node node_;

public:

	Phase(Node node) :
		node_(node)
	{
		/* void */
	}

	virtual void execute(Master& t, Slave& r, Slave& s) =0;
	virtual void terminate() =0;

	void confirm()
	{
		tcp_traits::confirm(node_.client());
	}

};

class RandomPhase : public Phase
{

public:

	RandomPhase(Node node) :
		Phase(node)
	{
		/* void */
	}

	virtual void execute(Master& t, Slave& r, Slave& s)
	{
		std::cout << "Processing: Random Phase" << std::endl;
		sleep((rand() % 6) + 1);
		std::cout << "Done." << std::endl;

		// confirm, await next command
		tcp_traits::confirm_await_command(node_.port(), node_.client());
	}

	virtual void terminate()
	{

	}

};

#endif  // SADDB_PHASE_H_
