#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "boost/archive/text_oarchive.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/tuple/tuple.hpp"

#include "track_join_data.h"

namespace
{
	std::string program_name;
}

void print_help()
{
	std::cout << "usage: " << program_name << " [port]" << std::endl;
}

template<typename C>
void print(const C& c)
{
	for (auto e : c)
		std::cout << e << std::endl;
}

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);

	if (argc < 3)
	{
		print_help();
		return 1;
	}

	// first argument contains program name
	program_name = args.front();
	args.erase(args.begin());

	// relation
	Relation relation;

	// read records
	std::ifstream ifs(args[0]);
	ifs >> relation;

	// serialize relation
	std::ostringstream archive_stream;
	boost::archive::text_oarchive archive(archive_stream);
	archive << relation;

	// save serialized relation
	std::ofstream ofs(args[1]);
	ofs << archive_stream.str();

	return 0;
}
