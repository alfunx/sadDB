#include <iostream>
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "address.h"
#include "node.h"
#include "master.h"
#include "slave.h"
#include "phase.h"
#include "tcp_traits.h"

namespace
{
	std::string program_name;
	Node node;
}

static void print_help()
{
	std::cout << "usage: " << program_name << " [port]" << std::endl;
}

static void execute_phase(Phase& p, Master& m, Slave& s1, Slave& s2)
{
	p.execute(m, s1, s2);
	p.terminate();
	p.confirm();
}

int main(int argc, char* argv[])
{
	std::vector<std::string> args(argv, argv + argc);

	// require at least one argument
	if (argc < 2)
	{
		print_help();
		return 1;
	}

	// first argument contains program name
	program_name = args.front();
	args.erase(args.begin());

	// set own details
	// TODO set own ip
	node.port(boost::lexical_cast<unsigned short>(args.front()))
		.ip("localhost");

	///////////////
	//  phase 0  //
	///////////////

	// wait for list of server addresses and client address
	AddressList servers;
	tcp_traits::await_broadcast(node.port(), servers);

	// extract client address
	node.client(servers.back());
	servers.pop_back();
	node.servers(servers).infer();

	// TODO remove
	for (auto s : *node.servers())
	{
		std::cout << s << ", id:" << s.id() << std::endl;
	}

	// initialize random number generator, include id in seed
	srand((unsigned)time(0) + node.id());

	// wait for database
	std::string database;
	tcp_traits::confirm_await_broadcast(node.port(), node.client(), database);
	std::cout << "database: " << database << std::endl;

	// initialize master and slaves
	Master m(node);
	Slave s1(node, R, database);
	Slave s2(node, S, database);

	// confirm, await next command
	tcp_traits::confirm_await_command(node.port(), node.client());

	// phase 1
	RandomPhase p1(node);
	execute_phase(p1, m, s1, s2);

	// phase 2
	RandomPhase p2(node);
	execute_phase(p2, m, s1, s2);

	// phase 3
	RandomPhase p3(node);
	execute_phase(p3, m, s1, s2);

	// finalize
	RandomPhase p4(node);
	execute_phase(p4, m, s1, s2);

	return 0;
}
