#include <functional>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "tcp_server.h"

int main(int argc, char* argv[])
{
	try
	{
		// Check command line arguments.
		if (argc != 2)
		{
			std::cerr << "Usage: server <port>" << std::endl;
			return 1;
		}
		unsigned short port = boost::lexical_cast<unsigned short>(argv[1]);

		boost::asio::io_service io_service;
		Server<std::vector<std::string>> server(io_service, port);

		// define exit condition (lambda)
		int count = 0;
		std::function<bool()> exit_condition = [&count]() {
			return !(++count < 2);
		};
		server.set_exit_condition(exit_condition);

		// start tcp server
		io_service.run();

		// print messages
		std::vector<std::vector<std::string>>* c = server.get_collection();
		for (auto e : *c)
			for (auto f : e)
				std::cout << f << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
