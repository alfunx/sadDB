#include <iostream>
#include <string>
#include <vector>

#include "address.h"
#include "node.h"
#include "tcp_traits.h"

namespace
{
	const static std::string ip_ = "localhost";
	const static unsigned short port_ = 12980;

	std::string program_name;
	Node node;
}

static void print_help()
{
	std::cout << "usage: " << program_name << " [ip:port [ip:port ...]]" << std::endl;
}

int main(int argc, char* argv[])
{
	std::vector<std::string> args(argv, argv + argc);

	// set client details
	node = Node(Address().ip(ip_).port(port_));
	std::cout << "client address: " << node << std::endl;

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
		node.add_server(Address(a));
	}

	// TODO remove
	for (auto s : *node.servers())
	{
		std::cout << s << std::endl;
	}

	// append client address to list of server addresses
	AddressList servers_and_client(*node.servers());
	servers_and_client.push_back(node);

	// broadcast list of server addresses and client address
	tcp_traits::broadcast_await_confirm(node.port(), *node.servers(), servers_and_client);
}
