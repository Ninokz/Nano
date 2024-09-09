#pragma once
#include <thread>
#include <chrono>
#include <iostream>
#include <string>
#include <mutex>
#include <memory>
#include <csignal>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "ceventhandler.h"
#include "conacceptor.h"
#include "session.h"
#include "nocopyable.h"

namespace Nano {
	namespace Communication {
		class BaseServer : public Noncopyable
		{
		public:
			typedef std::shared_ptr<BaseServer> Ptr;

			BaseServer(short port);
			virtual ~BaseServer();

			void Start();
		protected:
			boost::asio::io_context m_ioc;
			std::thread m_listenThread;

			std::unique_ptr<ConnectionAcceptor> m_acceptor;
			std::shared_ptr<CEventHandler> m_eventHandler;
		};
	}
}