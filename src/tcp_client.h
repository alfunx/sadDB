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

	unsigned int retry = 1000;

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

		bool error = true;
		while (error) {
			try
			{
				// start a synchronous connect operation
				boost::asio::connect(conn.socket(), endpoint_iterator);

				error = false;
			}
			catch (std::exception& e)
			{
				std::cerr << "Exception 1: " << e.what() << "\n";
			}
		}

		error = true;
		while (error) {
			try
			{
				// send object
				conn.sync_write(object,
						boost::bind(&TCP_Client::handle_write, this, boost::asio::placeholders::error));

				error = false;
			}
			catch (std::exception& e)
			{
				std::cerr << "Exception 2: " << e.what() << "\n";
			}
		}
	}

	TCP_Client(T& object, const std::string& host, const unsigned int& port) :
		TCP_Client(object, host, std::to_string(port))
	{
		/* void */
	}

	void start()
	{
		// ios.run();
	}

	void start(std::function<void()> f)
	{
		start();
		f();
	}

	void stop()
	{
		// ios.stop();
	}

private:

	void handle_write(const boost::system::error_code& e)
	{
		/* void */
	}

};

#endif  // SADDB_TCP_CLIENT_H_
