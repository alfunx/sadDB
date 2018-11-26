#include "record.h"

Record::Record()
{
	/* void */
}

Record::Record(Key k, std::vector<Payload> p) :
	key(k),
	payload(p)
{
	/* void */
}

std::ostream& operator <<(std::ostream& os, Record r)
{
	os << r.key;

	for (auto a : r.payload)
		os << "," << a;

	return os;
}

std::istream& operator >>(std::istream& is, Record& r)
{
	std::string k;
	std::getline(is, k);

	std::string s;
	std::getline(is, s);

	if (s.empty())
	{
		is.setstate(std::ios::badbit);
		return is;
	}

	r.key = boost::lexical_cast<int>(k);
	r.payload.push_back(s);

	return is;
}
