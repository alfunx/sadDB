#ifndef SADDB_NODE_H_
#define SADDB_NODE_H_

#include "address.h"

class Node : public Address
{

	AddressList servers_;
	Address client_;

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

	virtual Node& client(Address client)
	{
		client_ = client;
		return *this;
	}

	virtual Address client() const
	{
		return client_;
	}

	virtual Node& add_server(Address server)
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

};

#endif  // SADDB_NODE_H_
