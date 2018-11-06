#include <iostream>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "tcp_server.h"
#include "tcp_client.h"

void phase(int num) {
    // To lower the possibility of connection refused
    usleep(rand() % 10);
    std::cout << ".\n.\n.\nPhase " << num << " ends." << std::endl;
    std::string localhost = "0.0.0.0";
    std::string client_node_server = "8080";

//    int s = 1;
    try {
        Client<int> client(num, localhost, client_node_server);
        client.start();
        auto t = std::time(0);
        auto tm = *std::localtime(&t);
        std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void await_start(int phase_num, unsigned short port) {
    try
    {
        // Check command line arguments.
//        if (argc != 2)
//        {
//            std::cerr << "Usage: server <port>" << std::endl;
//            return 1;
//        }
//        unsigned short port = boost::lexical_cast<unsigned short>(argv[1]);
        Server<int> server(port);

        // define exit condition (counter)
        server.set_exit_condition(1);

        // start tcp server
        server.start();

        // print messages
        std::vector<int>* c = server.get_collection();
        for (auto e : *c)
            if (e == phase_num) {
                std::cout << "Phase "<< phase_num << std::endl;
                phase(phase_num);
            }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        // try again
//        await_start(phase_num, port);
    }
}

int main(int argc, char* argv[])
{
	std::cout << "Hello, sadDB! Waiting for phase." << std::endl;
    // Check command line arguments.
    if (argc != 2)
    {
        std::cerr << "Usage: server <port>" << std::endl;
        return 1;
    }
    unsigned short port = boost::lexical_cast<unsigned short>(argv[1]);


	try
	{

//		unsigned short port = boost::lexical_cast<unsigned short>(argv[1]);
		Server<int> server(port);

		// define exit condition (counter)
		server.set_exit_condition(1);

		// start tcp server
		server.start();

		// print messages
		std::vector<int>* c = server.get_collection();
		for (auto e : *c)
			if (e == 1) {
			    std::cout << "Phase 1" << std::endl;
			    phase(1);
			}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}


    await_start(2, port);

	return 0;
}


