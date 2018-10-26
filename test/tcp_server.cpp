#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "connection.h"
#include "tcp_server.h"

int main(int argc, char* argv[])
{
	try
	{
		// check command line arguments
		if (argc != 2)
		{
			std::cerr << "Usage: server <port>" << std::endl;
			return 1;
		}
		unsigned short port = boost::lexical_cast<unsigned short>(argv[1]);

		// vector to collect strings of received vectors
		std::vector<std::string> result;

		// initialize tcp server with callback for incoming objects
		typedef std::vector<std::string> T;

		Server<T> server(port, [&result](boost::shared_ptr<T> t, ConnectionPtr c) {
			// print ip address and port
			std::cout << ">>> received from: "
				<< c->socket().remote_endpoint().address()
				<< ":"
				<< c->socket().remote_endpoint().port()
				<< std::endl;

			for (auto e : *t)
			{
				// print elements of received vector
				std::cout << e << std::endl;

				// append elements of received vector to result
				result.push_back(e);
			}

			std::cout << std::endl;
		});

		// set expected number of incoming objects
		server.expected_quantity(2);

		// start tcp server
		server.start();

		// print elements of result vector
		std::cout << ">>> result:" << std::endl;
		for (auto e : result)
			std::cout << e << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
