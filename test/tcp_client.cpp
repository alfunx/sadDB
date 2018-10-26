#include <iostream>
#include <iomanip>
#include <ctime>

#include "tcp_client.h"

int main(int argc, char* argv[])
{
	try
	{
		// check command line arguments
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

		// initialize and start client
		Client<std::vector<std::string>> client(s, argv[1], argv[2]);
		client.start();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
