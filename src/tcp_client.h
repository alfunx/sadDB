#ifndef SADDB_TCP_CLIENT_H_
#define SADDB_TCP_CLIENT_H_

#include <iostream>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "connection.h" // must come before boost/serialization headers
#include <boost/serialization/vector.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;

template <typename T>
class TCP_Client
{

	io_service ios;
	Connection conn;
	T& object;

	const std::string host;
	const std::string service;
	tcp::resolver::iterator endpoint_iterator;

public:

	TCP_Client(T& object, const std::string& host, const std::string& service) :
		conn(ios),
		object(object),
		host(host),
		service(service)
	{
		// resolve the host name into an IP address
		tcp::resolver resolver(ios);
		tcp::resolver::query query(host, service);
		endpoint_iterator = resolver.resolve(query);

		// start an asynchronous connect operation
		boost::asio::async_connect(conn.socket(),
				endpoint_iterator,
				boost::bind(&TCP_Client::handle_connect, this, boost::asio::placeholders::error));
	}

	TCP_Client(T& object, const std::string& host, const unsigned int& port) :
		TCP_Client(object, host, std::to_string(port))
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

private:

	/// handle completion of a connect operation
	void handle_connect(const boost::system::error_code& e)
	{
		if (!e)
		{
			// successfully established connection, start writing
			conn.async_write(object,
					boost::bind(&TCP_Client::handle_write, this, boost::asio::placeholders::error));
		}
		else
		{
			std::cerr << e.message() << " - Retry..." << std::endl;

			// start an asynchronous connect operation
			boost::asio::async_connect(conn.socket(),
					endpoint_iterator,
					boost::bind(&TCP_Client::handle_connect, this, boost::asio::placeholders::error));
		}
	}

	void handle_write(const boost::system::error_code& e)
	{
		/* void */
	}

};

#endif  // SADDB_TCP_CLIENT_H_
