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

		Server<std::vector<std::string>> server(port);

		// define exit condition (counter)
		server.set_exit_condition(2);

		// start tcp server
		server.start();

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
