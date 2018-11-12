#ifndef SADDB_NODE_H_
#define SADDB_NODE_H_

#include "address.h"

class Node : public Address
{

	AddressList servers_;
	Address client_;
	std::string database_;

public:

	static const unsigned short port_count = 6;

	Node()
	{
		/* void */
	}

	Node(const Address& address) : Address(address)
	{
		/* void */
	}

	virtual Node& infer()
	{
		for (auto a : *servers())
		{
			if (a.port() == port() && a.ip() == ip())
			{
				id(a.id());
				break;
			}
		}

		return *this;
	}

	virtual Node& client(const Address& client)
	{
		client_ = client;
		return *this;
	}

	virtual Address client() const
	{
		return client_;
	}

	virtual Node& add_server(const Address& server)
	{
		servers_.push_back(server);
		return *this;
	}

	virtual Node& servers(AddressList servers)
	{
		servers_ = servers;
		return *this;
	}

	virtual const AddressList* servers() const
	{
		return &servers_;
	}

	virtual Node& database(const std::string database)
	{
		database_ = database;
		return *this;
	}

	virtual std::string database() const
	{
		return database_;
	}

	virtual Address get_address(unsigned int id) const
	{
		for (auto a : *servers())
		{
			if (a.id() == id)
			{
				return a;
			}
		}

		return Address();
	}

};

#endif  // SADDB_NODE_H_
