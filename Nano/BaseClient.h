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
			BaseClient();
			virtual ~BaseClient();

			bool Connect(const std::string& ip, short port);
			void Disconnect();
			void Send(const char* data, int dataLength);
		public:
			std::shared_ptr<CEventHandler> m_ceventHandler;
		protected:
			bool m_running;
			boost::asio::ip::tcp::endpoint m_endpoint;

			std::thread m_clientThread;
			boost::asio::io_context m_ioc;
			std::shared_ptr<Session> m_session;
		};
	}
}