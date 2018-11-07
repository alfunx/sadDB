#include <iostream>
#include <string>
#include <vector>

#include "address.h"
#include "tcp_traits.h"

namespace
{
	std::string program_name;
	static const unsigned short main_port = 12000;
	static const Address client_address("localhost", main_port);
	AddressList servers;
}

static void print_help()
{
	std::cout << "usage: " << program_name << " [ip:port [ip:port ...]]" << std::endl;
}

int main(int argc, char* argv[])
{
	std::vector<std::string> args(argv, argv + argc);

	// first argument contains program name
	program_name = args.front();
	args.erase(args.begin());

	// require at least one server
	if (argc < 1)
	{
		print_help();
		return 1;
	}

	// insert server addresses
	for (auto a : args)
	{
		servers.push_back(a);
	}

	// TODO remove
	for (auto s : servers)
	{
		std::cout << s << std::endl;
	}

	// append client address to list of server addresses
	AddressList servers_and_client(servers);
	servers_and_client.push_back(client_address);

	// broadcast list of server addresses and client address
	tcp_traits::broadcast_await_confirm(main_port, servers, servers_and_client);
}
