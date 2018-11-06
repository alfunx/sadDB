#ifndef CLIENT_H_
#define CLIENT_H_

#include <iostream>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "connection.h" // must come before boost/serialization headers
#include <boost/serialization/vector.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;

template <typename T>
class Client
{

private:

	io_service ios;
	Connection conn;
	T& object;

public:

	Client(T& object, const std::string& host, const std::string& service) :
		conn(ios),
		object(object)
	{
		// resolve the host name into an IP address
		tcp::resolver resolver(ios);
		tcp::resolver::query query(host, service);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		// start an asynchronous connect operation
		boost::asio::async_connect(conn.socket(),
				endpoint_iterator,
				boost::bind(&Client::handle_connect, this, boost::asio::placeholders::error));
	}

	void start()
	{
		ios.run();
	}

	void stop()
	{
		ios.stop();
	}

private:

	/// handle completion of a connect operation
	void handle_connect(const boost::system::error_code& e)
	{
		if (!e)
		{
			// successfully established connection, start writing
			conn.async_write(object,
					boost::bind(&Client::handle_write, this, boost::asio::placeholders::error));
		}
		else
		{
			std::cerr << e.message() << std::endl;
		}
	}

	void handle_write(const boost::system::error_code& e)
	{
		/* void */
	}

};

#endif  // CLIENT_H_
