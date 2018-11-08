#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "address.h"
#include "tcp_traits.h"

namespace
{
	std::string program_name;
	static const unsigned short port_count = 6;
	unsigned short main_port;
	AddressList servers;
	Address client_address;
}

static void print_help()
{
	std::cout << "usage: " << program_name << " [port]" << std::endl;
}

int main(int argc, char* argv[])
{
	std::vector<std::string> args(argv, argv + argc);

	// first argument contains program name
	program_name = args.front();
	args.erase(args.begin());

	// require at least one argument
	if (argc < 1)
	{
		print_help();
		return 1;
	}

	// set own port
	main_port = boost::lexical_cast<unsigned short>(args.front());

	// wait for list of server addresses and client address
	tcp_traits::await_broadcast(main_port, servers);

	// extract client address
	client_address = servers.back();
	servers.pop_back();

	// confirm client
	tcp_traits::confirm(client_address);

	// TODO remove
	for (auto s : servers)
	{
		std::cout << s << ", id:" << s.id() << std::endl;
	}
}
