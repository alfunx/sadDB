#ifndef SADDB_RECORD_H_
#define SADDB_RECORD_H_

#include <set>
#include <string>
#include <vector>

#include <boost/serialization/map.hpp>
#include <boost/lexical_cast.hpp>

typedef int Key;
typedef std::string Payload;

class Record
{

public:

	Key key;
	std::vector<Payload> payload;

	Record();
	Record(Key k, std::vector<Payload> p);

	friend std::ostream& operator <<(std::ostream& os, Record r);
	friend std::istream& operator >>(std::istream& is, Record& r);

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & key;
		ar & payload;
	}

};

#endif  // SADDB_RECORD_H_
