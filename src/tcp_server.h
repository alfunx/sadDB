#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "connection.h" // must come before boost/serialization headers
#include <boost/serialization/vector.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;

template <typename T>
class Server
{

	io_service& ios;
	tcp::acceptor acceptor;
	std::vector<T>* collection;

public:

	Server(io_service& ios, unsigned short port) :
		ios(ios),
		acceptor(ios, tcp::endpoint(tcp::v4(), port))
	{
		collection = new std::vector<T>();

		// start an accept operation for a new connection
		ConnectionPtr new_conn(new Connection(acceptor.get_io_service()));
		acceptor.async_accept(new_conn->socket(),
				boost::bind(&Server::handle_accept, this, boost::asio::placeholders::error, new_conn));
	}

	~Server()
	{
		delete collection;
	}

	void handle_accept(const boost::system::error_code& e, ConnectionPtr conn)
	{
		if (!e)
		{
			// successfully established connection, start reading
			boost::shared_ptr<T> t(new T());
			conn->async_read(*t,
					boost::bind(&Server::handle_read, this, boost::asio::placeholders::error, conn, t));
		}

		// start an accept operation for a new connection
		ConnectionPtr new_conn(new Connection(acceptor.get_io_service()));
		acceptor.async_accept(new_conn->socket(),
				boost::bind(&Server::handle_accept, this, boost::asio::placeholders::error, new_conn));
	}

	/// handle completion of a read operation
	void handle_read(const boost::system::error_code& e, ConnectionPtr conn, boost::shared_ptr<T> t)
	{
		if (!e)
		{
			collection->push_back(*t);
		}
		else
		{
			std::cerr << e.message() << std::endl;
		}
	}

	T* get_collection()
	{
		return collection;
	}

};

#endif  // SERVER_H_
