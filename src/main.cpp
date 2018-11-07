#include <iostream>
#include <thread>

#include "slave.h"

int main(int argc, char* argv[])
{
	std::cout << "Hello, sadDB!" << std::endl;

	// This should be filled when the program is started
	const std::string experiment = "experiment1";
	const std::string node_nr = "node1";

	Slave process_R(R, experiment, node_nr);

	Slave process_S(S, experiment, node_nr);

	/* This is the first phase*/
	std::thread r1([&] (Slave * process) {process->phase1();}, &process_R);
	std::thread s1([&] (Slave * process) {process->phase1();}, &process_S);

	r1.join();
	s1.join();

	// Send msg to client
	/* End of first phase */

	/* Second phase */
    std::thread r2([&] (Slave * process) {process->phase2();}, &process_R);
    std::thread s2([&] (Slave * process) {process->phase2();}, &process_S);

    r2.join();
    s2.join();

	// Send msg to client
	/* End second phase */

	return 0;

}
