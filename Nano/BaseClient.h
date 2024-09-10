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

#include "nocopyable.h"
#include "ceventhandler.h"
#include "session.h"
#include "const.h"

namespace Nano {
	namespace Communication {
		class BaseClient
		{
		public:
			typedef std::shared_ptr<BaseClient> Ptr;

		protected:
			bool m_running;
			boost::asio::ip::tcp::endpoint m_endpoint;

			std::thread m_clientThread;
			boost::asio::io_context m_ioc;
			std::shared_ptr<Session> m_session;
			std::shared_ptr<CEventHandler> m_ceventHandler;
		};
	}
}