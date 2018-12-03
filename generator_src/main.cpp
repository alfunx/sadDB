#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
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
	std::cout << "usage: " << program_name << " [-i <file>] [-o <file>]"
		<< " [-p <payload>] [-t <tuples>] [-r <min> <max>]" << std::endl;
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

	// first argument contains program name
	program_name = args.front();
	args.erase(args.begin());

	if (argc < 1)
	{
		print_help();
		return 1;
	}

	// options
	std::string input;
	std::string output;

	// options for random generator
	unsigned int n_payload = 0;
	unsigned int n_tuples = 0;
	int min = 0;
	int max = 0;

	for (int i = 0; i < args.size(); ++i)
	{
		if ("-i" == args[i])
		{
			input = args[++i];
		}

		else if ("-o" == args[i])
		{
			output = args[++i];
		}

		else if ("-p" == args[i])
		{
			n_payload = boost::lexical_cast<unsigned int>(args[++i]);
		}

		else if ("-t" == args[i])
		{
			n_tuples = boost::lexical_cast<unsigned int>(args[++i]);
		}

		else if ("-r" == args[i])
		{
			min = boost::lexical_cast<int>(args[++i]);
			max = boost::lexical_cast<int>(args[++i]);
		}

		else if ("-h" == args[i])
		{
			print_help();
			return 0;
		}

		else
		{
			print_help();
			return 1;
		}
	}

	// relation
	Relation relation;

	// if input file given
	if (!input.empty())
	{
		// read records
		std::ifstream ifs(input);
		ifs >> relation;
	}
	else
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_int_distribution<int> distribution(min, max);

		std::string payload;
		for (int i = 0; i < n_payload; ++i)
			payload.push_back('.');

		relation.reserve(n_tuples);
		for (int i = 0; i < n_tuples; ++i)
		{
			relation.insert(Record(distribution(generator),
						std::vector { payload }));
		}
	}

	// serialize relation
	std::ostringstream archive_stream;
	boost::archive::text_oarchive archive(archive_stream);
	archive << relation;

	// save serialized relation
	std::ofstream ofs(output);
	ofs << archive_stream.str();

	return 0;
}
