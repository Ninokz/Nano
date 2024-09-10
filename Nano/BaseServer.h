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
#include "const.h"
#include "sessionmanager.h"
#include "nocopyable.h"
#include "Log.h"

using namespace Nano::Log;
namespace Nano {
	namespace Communication {
		class BaseServer : public Noncopyable
		{
		public:
			typedef std::shared_ptr<BaseServer> Ptr;
			BaseServer(short port);
			virtual ~BaseServer();
			void Start();

			std::shared_ptr<CEventHandler> m_ceventHandler;
		protected:
			bool m_running;
			short m_port;
			boost::asio::io_context m_ioc;
			std::thread m_listenThread;

			std::shared_ptr<SessionManager> m_sessionManager;
			std::unique_ptr<ConnectionAcceptor> m_acceptor;
		};
	}
}