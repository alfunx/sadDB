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

	virtual Address& ip(std::string ip)
	{
		ip_ = ip;
		return *this;
	}

	virtual Address& service(std::string service)
	{
		service_ = service;
		return *this;
	}

	virtual Address& port(unsigned short port)
	{
		service_ = std::to_string(port);
		return *this;
	}

	virtual Address& id(unsigned int id)
	{
		id_ = id;
		return *this;
	}

	virtual std::string ip() const
	{
		return ip_;
	}

	virtual std::string service() const
	{
		return service_;
	}

	virtual unsigned short port() const
	{
		return boost::lexical_cast<unsigned short>(service_);
	}

	virtual unsigned int id() const
	{
		return id_;
	}

};

typedef std::vector<Address> AddressList;

#endif  // SADDB_ADDRESS_H_
