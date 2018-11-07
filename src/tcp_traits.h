#ifndef TCP_TRAITS_H_
#define TCP_TRAITS_H_

#include "address.h"
#include "tcp_client.h"
#include "tcp_server.h"

struct tcp_traits
{

	static void confirm(Address address)
	{
		bool status = true;
		Client<bool> tcp_client(status, address.ip(), address.service());
		tcp_client.start();
	}

	static void await_confirm(unsigned short port, unsigned int count)
	{
		Server<bool> tcp_server(port, [&count](boost::shared_ptr<bool> p, ConnectionPtr c) {});
		tcp_server.expected_quantity(count);
		tcp_server.start();

		// TODO remove
		std::cout << "Received " << count << " confirmations." << std::endl;
	}

	template <typename T>
	static void await_broadcast(unsigned short port, T& t)
	{
		// wait for incoming broadcast
		Server<T> tcp_server(port, [&t](boost::shared_ptr<T> p, ConnectionPtr c) {
			t = *p;
		});
		tcp_server.expected_quantity(1);
		tcp_server.start();
	}

	template <typename A, typename T>
	static void broadcast_await_confirm(unsigned short port, A& recipients, T& t)
	{
		// wait for confirmations (in thread)
		boost::thread confirm_thread {
			boost::bind(&tcp_traits::await_confirm, port, recipients.size())
		};

		// send payload to recipients
		for (auto r : recipients)
		{
			Client<T> tcp_client(t, r.ip(), r.service());
			tcp_client.start();
		}

		// join thread
		confirm_thread.join();
	}

	template <typename T>
	static void await_broadcast_confirm(unsigned short port, Address broadcaster, T& t)
	{
		// wait for incoming broadcast
		Server<T> tcp_server(port, [&t](boost::shared_ptr<T> p, ConnectionPtr c) {
			t = *p;
		});
		tcp_server.expected_quantity(1);
		tcp_server.start();

		// confirm recipience
		tcp_traits::confirm(broadcaster);
	}

};

#endif  // TCP_TRAITS_H_
