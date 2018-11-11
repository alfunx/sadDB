#ifndef SADDB_ADDRESS_H_
#define SADDB_ADDRESS_H_

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/lexical_cast.hpp>

static int ADDRESS_ID_COUNTER = -1;

class Address
{

	std::string ip_;
	std::string service_;
	unsigned int id_;

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & ip_;
		ar & service_;
		ar & id_;
	}

	friend std::ostream& operator <<(std::ostream& os, Address a)
	{
		os << a.ip() << ':' << a.service();
		return os;
	}

public:

	Address()
	{
		/* void */
	}

	Address(std::string ip, unsigned int port) :
		ip_(ip),
		service_(std::to_string(port)),
		id_(++ADDRESS_ID_COUNTER)
	{
		/* void */
	}

	Address(std::string ip, std::string service) :
		ip_(ip),
		service_(service),
		id_(++ADDRESS_ID_COUNTER)
	{
		/* void */
	}

	Address(const Address& address) :
		ip_(address.ip()),
		service_(address.service()),
		id_(address.id())
	{
		/* void */
	}

	Address(std::string address) :
		id_(++ADDRESS_ID_COUNTER)
	{
		std::vector<std::string> part;
		boost::split(part, address, [](char c) { return c == ':'; } );

		if (part.size() != 2)
		{
			throw std::invalid_argument("Invalid formatting of address.");
		}

		ip_ = part[0];
		service_ = part[1];
	}

	std::string ip() const
	{
		return ip_;
	}

	std::string service() const
	{
		return service_;
	}

	unsigned short port() const
	{
		return boost::lexical_cast<unsigned short>(service_);
	}

	unsigned int id() const
	{
		return id_;
	}

};

typedef std::vector<Address> AddressList;

#endif  // SADDB_ADDRESS_H_
