#ifndef MEASUREMENT_TRAITS
#define MEASUREMENT_TRAITS

#include <algorithm>
#include <numeric>
#include <ostream>

#include <boost/lexical_cast.hpp>
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/tuple/tuple.hpp"

struct measurement_traits
{

	template <typename T>
	static size_t size(T& t)
	{
		std::ostringstream archive_stream;
		boost::archive::text_oarchive archive(archive_stream);
		archive << t;

		return archive_stream.str().size();
	}

	template <typename T>
	static void print_stats(std::ostream& os, T& t)
	{
		os << "- Count: " << t.size() << std::endl;
		if (t.empty())
		{
			os << "- Total: " << 0 << std::endl;
			os << "- Average: " << 0 << std::endl;
			os << "- Min: " << 0 << std::endl;
			os << "- Max: " << 0 << std::endl;
		}
		else
		{
			os << "- Total: " << std::accumulate(t.begin(), t.end(), 0) << std::endl;
			os << "- Average: " << std::accumulate(t.begin(), t.end(), 0) / t.size() << std::endl;
			os << "- Min: " << *std::min_element(t.begin(), t.end()) << std::endl;
			os << "- Max: " << *std::max_element(t.begin(), t.end()) << std::endl;
		}
	}

	template <typename T>
	static void _print_all_stats(std::ostream& os, T& p2, T& p3)
	{
		if (p2.empty())
		{
			os << p2.size() << "," << "," << "," << "," << ",";
		}
		else
		{
			os << p2.size() << ","
				<< std::accumulate(p2.begin(), p2.end(), 0) << ","
				<< std::accumulate(p2.begin(), p2.end(), 0) / p2.size() << ","
				<< *std::min_element(p2.begin(), p2.end()) << ","
				<< *std::max_element(p2.begin(), p2.end()) << ",";
		}

		if (p3.empty())
		{
			os << p3.size() << "," << "," << "," << "," << "," << std::endl;
		}
		else
		{
			os << p3.size() << ","
				<< std::accumulate(p3.begin(), p3.end(), 0) << ","
				<< std::accumulate(p3.begin(), p3.end(), 0) / p3.size() << ","
				<< *std::min_element(p3.begin(), p3.end()) << ","
				<< *std::max_element(p3.begin(), p3.end())
				<< std::endl;
		}
	}

	template <typename T>
	static void _print_all_stats(std::ostream& os, T& p3)
	{
		os << "," << "," << "," << "," << ",";
		if (p3.empty())
		{
			os << p3.size() << "," << "," << "," << "," << "," << std::endl;
		}
		else
		{
			os << p3.size() << ","
				<< std::accumulate(p3.begin(), p3.end(), 0) << ","
				<< std::accumulate(p3.begin(), p3.end(), 0) / p3.size() << ","
				<< *std::min_element(p3.begin(), p3.end()) << ","
				<< *std::max_element(p3.begin(), p3.end())
				<< std::endl;
		}
	}

	template <typename T>
	static void print_all_stats(std::ostream& os, T& m3, T& r2, T& r3, T& s2, T& s3)
	{
		_print_all_stats(os, m3);
		_print_all_stats(os, r2, r3);
		_print_all_stats(os, s2, s3);
	}

};

#endif  // MEASUREMENT_TRAITS
