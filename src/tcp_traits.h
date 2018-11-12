#ifndef SADDB_TCP_TRAITS_H_
#define SADDB_TCP_TRAITS_H_

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "address.h"
#include "tcp_client.h"
#include "tcp_server.h"

struct tcp_traits
{

	static void confirm(Address address)
	{
		bool status = true;
		TCP_Client<bool> tcp_client(status, address.ip(), address.service());
		tcp_client.start();
	}

	static void await_confirm(unsigned short port, unsigned int count)
	{
		TCP_Server<bool> tcp_server(port, [&count](boost::shared_ptr<bool> p, ConnectionPtr c) {});
		tcp_server.expected_quantity(count);
		tcp_server.start();
	}

	template <typename T>
	static void await_broadcast(unsigned short port, T& t)
	{
		// wait for incoming broadcast
		TCP_Server<T> tcp_server(port, [&t](boost::shared_ptr<T> p, ConnectionPtr c) {
			t = *p;
		});
		tcp_server.expected_quantity(1);
		tcp_server.start();
	}

	template <typename A, typename T>
	static void broadcast(unsigned short port, A& recipients, T& t)
	{
		// send payload to recipients
		for (auto r : recipients)
		{
			TCP_Client<T> tcp_client(t, r.ip(), r.service());
			tcp_client.start();
		}
	}

	template <typename A, typename T>
	static void broadcast_await_confirm(unsigned short port, A& recipients, T& t)
	{
		// wait for confirmations (in thread)
		boost::thread confirm_thread {
			boost::bind(&tcp_traits::await_confirm, port, recipients.size())
		};

		// TODO remove
		sleep(1);

		// broadcast
		tcp_traits::broadcast(port, recipients, t);

		// join thread
		confirm_thread.join();
	}

	template <typename T>
	static void await_broadcast_confirm(unsigned short port, Address broadcaster, T& t)
	{
		// wait for incoming broadcast
		TCP_Server<T> tcp_server(port, [&t](boost::shared_ptr<T> p, ConnectionPtr c) {
			t = *p;
		});
		tcp_server.expected_quantity(1);
		tcp_server.start();

		// confirm recipience
		tcp_traits::confirm(broadcaster);
	}

	template <typename T>
	static void confirm_await_broadcast(unsigned short port, Address broadcaster, T& t)
	{
		// wait for broadcast (in thread)
		boost::thread confirm_thread {
			boost::bind(&tcp_traits::await_broadcast<T>, port, boost::ref(t))
		};

		// TODO remove
		sleep(1);

		// confirm
		tcp_traits::confirm(broadcaster);

		// join thread
		confirm_thread.join();
	}

	template <typename A>
	static void command_await_confirm(unsigned short port, A& recipients)
	{
		bool command = 0;
		broadcast_await_confirm(port, recipients, command);
	}

	static void confirm_await_command(unsigned short port, Address broadcaster)
	{
		bool command;
		confirm_await_broadcast(port, broadcaster, command);
	}

};

#endif  // SADDB_TCP_TRAITS_H_
