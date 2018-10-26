#include <iostream>
#include <iomanip>
#include <ctime>

#include "tcp_client.h"

int main(int argc, char* argv[])
{
	try
	{
		// Check command line arguments.
		if (argc != 3)
		{
			std::cerr << "Usage: client <host> <port>" << std::endl;
			return 1;
		}

		std::vector<std::string> s;

		// append message
		std::cout << ">>> ";
		std::string str;
		std::getline(std::cin, str);
		s.push_back("message:      " + str);

		// append timestamp
		auto t = std::time(0);
		auto tm = *std::localtime(&t);
		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		s.push_back("current time: " + oss.str());

		boost::asio::io_service io_service;
		Client<std::vector<std::string>> client(s, io_service, argv[1], argv[2]);
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
