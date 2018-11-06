#include <iostream>
#include <vector>
#include <string>

#include <boost/lexical_cast.hpp>

#include "tcp_client.h"
#include "tcp_server.h"

void start_phase(int phase_num) {
    std::string localhost = "0.0.0.0";
    std::string nodes[] = {"8081", "8082", "8083"};
    for (const auto &node : nodes) {
        try {
            Client<int> client(phase_num, localhost, node);
            client.start();
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

void await_end(int phase_num) {
    std::string client_node = "8080";
    try
    {
        unsigned short port = boost::lexical_cast<unsigned short>(client_node);
        Server<int> server(port);

        // define exit condition (counter)
        server.set_exit_condition(3);

        // start tcp server
        server.start();

        // print messages
        std::vector<int>* c = server.get_collection();
        for (auto e : *c)
            if (e == phase_num)
                std::cout << "End of Phase "<< phase_num << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
	std::cout << "Hello, sadDB_client!" << std::endl;
    /* Start query (Track Join) - phase 1 */
    start_phase(1);

    /* Wait for nodes to finish phase 1 */
    await_end(1);

    /* Phase 2 */
    start_phase(2);

    /* Wait for nodes to finish phase 2 */
    await_end(2);
}
