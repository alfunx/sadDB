#ifndef SADDB_TCP_SERVER_H_
#define SADDB_TCP_SERVER_H_

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "connection.h" // must come before boost/serialization headers
#include <boost/serialization/vector.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;

template <typename T>
class TCP_Server
{

	io_service ios;
	tcp::acceptor acceptor;

	std::function<void(boost::shared_ptr<T>, ConnectionPtr)> handle_object =
		[](boost::shared_ptr<T> t, ConnectionPtr) { return; };

	int counter = -1;

public:

	TCP_Server(unsigned short port, std::function<void(boost::shared_ptr<T>, ConnectionPtr)> f) :
		acceptor(ios, tcp::endpoint(tcp::v4(), port))
	{
		// hanle an incoming object
		this->handle_object = f;

		// start an accept operation for a new connection
		ConnectionPtr new_conn(new Connection(acceptor.get_io_service()));
		acceptor.async_accept(new_conn->socket(),
				boost::bind(&TCP_Server::handle_accept, this, boost::asio::placeholders::error, new_conn));
	}

	~TCP_Server()
	{
		/* void */
	}

	void start()
	{
		ios.run();
	}

	void stop()
	{
		ios.stop();
	}

	/// must be greater than zero
	void expected_quantity(int counter)
	{
		assert(counter > 0);
		this->counter = counter;
	}

private:

	void handle_accept(const boost::system::error_code& e, ConnectionPtr conn)
	{
		if (!e)
		{
			// successfully established connection, start reading
			boost::shared_ptr<T> t(new T());
			conn->async_read(*t,
					boost::bind(&TCP_Server::handle_read, this, boost::asio::placeholders::error, conn, t));
		}

		if (counter > 0 && --counter == 0)
			return;

		// start an accept operation for a new connection
		ConnectionPtr new_conn(new Connection(acceptor.get_io_service()));
		acceptor.async_accept(new_conn->socket(),
				boost::bind(&TCP_Server::handle_accept, this, boost::asio::placeholders::error, new_conn));
	}

	/// handle completion of a read operation
	void handle_read(const boost::system::error_code& e, ConnectionPtr conn, boost::shared_ptr<T> t)
	{
		if (!e)
		{
			handle_object(t, conn);
		}
		else
		{
			std::cerr << e.message() << std::endl;
		}
	}

};

#endif  // SADDB_TCP_SERVER_H_
