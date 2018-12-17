#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "address.h"
#include "measurement_traits.h"
#include "node.h"
#include "relation.h"
#include "tcp_traits.h"
#include "track_join_data.h"

#include "master.h"
#include "slave.h"

#include "phase.h"
#include "phase_1.h"
#include "phase_2.h"
#include "phase_3.h"

namespace
{
	std::string program_name;
	Node node;
}

static void print_help()
{
	std::cout << "usage: " << program_name << " [port] [-o file.csv]" << std::endl;
}

static void execute_phase(Phase& p)
{
	p.execute();
	p.terminate();
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

	// options
	std::string csv_file;

	for (int i = 1; i < args.size(); ++i)
	{
		if ("-o" == args[i])
		{
			csv_file = args[++i];
		}

		else if ("-h" == args[i])
		{
			print_help();
			return 0;
		}

		else
		{
			print_help();
			return 1;
		}
	}

	// wait for list of server addresses and client address
	AddressList servers;
	tcp_traits::await_broadcast(node.port(), servers);

	// extract client address
	node.client(servers.back());
	servers.pop_back();
	node.servers(servers).infer();
	std::cerr << "me: " << node << std::endl;

	// TODO remove
	for (auto s : *node.servers())
	{
		std::cerr << s << ", id:" << s.id() << std::endl;
	}

	// initialize random number generator, include id in seed
	srand((unsigned)time(0) + node.id());

	// wait for database
	std::string database;
	tcp_traits::confirm_await_broadcast(node.port(), node.client(), database);
	node.database(database);
	std::cerr << "database: " << database << std::endl;

	// initialize master and slaves
	Master master(node);
	Slave slave_r(node, Relation::Type::R);
	Slave slave_s(node, Relation::Type::S);

	// confirm, await next command
	tcp_traits::confirm_await_command(node.port(), node.client());

	// phase 1
	Phase_1 p1(node, master, slave_r, slave_s);
	execute_phase(p1);

	// phase 2
	Phase_2 p2(node, master, slave_r, slave_s);
	execute_phase(p2);

	// phase 3
	Phase_3 p3(node, master, slave_r, slave_s);
	execute_phase(p3);

	std::cerr << "Master phase 3 messages: " << master.phase_3_sent_messages_count << std::endl;
	measurement_traits::print_stats(std::cerr, master.phase_3_sent_messages_sizes);

	std::cerr << "Slave R phase 2 messages: " << slave_r.phase_2_sent_messages_count << std::endl;
	measurement_traits::print_stats(std::cerr, slave_r.phase_2_sent_messages_sizes);

	std::cerr << "Slave R phase 3 messages: " << slave_r.phase_3_sent_messages_count << std::endl;
	measurement_traits::print_stats(std::cerr, slave_r.phase_3_sent_messages_sizes);

	std::cerr << "Slave S phase 2 messages: " << slave_s.phase_2_sent_messages_count << std::endl;
	measurement_traits::print_stats(std::cerr, slave_s.phase_2_sent_messages_sizes);

	std::cerr << "Slave S phase 3 messages: " << slave_s.phase_3_sent_messages_count << std::endl;
	measurement_traits::print_stats(std::cerr, slave_s.phase_3_sent_messages_sizes);

	if (!csv_file.empty())
	{
		std::ofstream ofs(csv_file);
		measurement_traits::print_all_stats(ofs,
				master.phase_3_sent_messages_sizes,
				slave_r.phase_2_sent_messages_sizes,
				slave_r.phase_3_sent_messages_sizes,
				slave_s.phase_2_sent_messages_sizes,
				slave_s.phase_3_sent_messages_sizes);
	}

	return 0;
}
