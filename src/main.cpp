#include <iostream>
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "address.h"
#include "node.h"
// #include "master.h"
// #include "slave.h"
#include "tcp_traits.h"
#include "track_join_data.h"

#include "phase.h"
#include "phase_1.h"
#include "phase_2.h"

namespace
{
	std::string program_name;
	Node node;
}

static void print_help()
{
	std::cout << "usage: " << program_name << " [port]" << std::endl;
}

static void execute_phase(Phase& p, TJD& data)
{
	p.execute(data);
	p.terminate(data);
	// p.confirm();
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

	// wait for list of server addresses and client address
	AddressList servers;
	tcp_traits::await_broadcast(node.port(), servers);

	// extract client address
	node.client(servers.back());
	servers.pop_back();
	node.servers(servers).infer();
	std::cout << "me: " << node << std::endl;

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
	node.database(database);
	std::cout << "database: " << database << std::endl;

	// // initialize master and slaves
	// Master m(node);
	// Slave s1(node, R, database, "");
	// Slave s2(node, S, database, "");

	// confirm, await next command
	tcp_traits::confirm_await_command(node.port(), node.client());

	// TODO
	TJD data;

	// phase 1
	Phase_1 p1(node);
	execute_phase(p1, data);

	// phase 2
	Phase_2 p2(node);
	execute_phase(p2, data);

	// phase 3
	RandomPhase p3(node);
	execute_phase(p3, data);

	// finalize
	RandomPhase p4(node);
	execute_phase(p4, data);

	return 0;
}
