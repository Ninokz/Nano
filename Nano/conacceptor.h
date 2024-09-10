#pragma once
#include <mutex>
#include <memory>
#include <string>
#include <iostream>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "ceventhandler.h"
#include "ioServicePool.h"

namespace Nano {
	namespace Communication {
		class ConnectionAcceptor {
		public:
			typedef std::shared_ptr<ConnectionAcceptor> Ptr;
		public:
			ConnectionAcceptor(boost::asio::io_context& ioc, short port,CEventHandler& ceventHandler);
			~ConnectionAcceptor();

			void StartAccept();
			void StopAccept();
		private:
			void HandleAccept(std::shared_ptr<Session> new_session, const boost::system::error_code& ec);
		private:
			boost::asio::ip::tcp::acceptor m_acceptor;
			boost::asio::io_context& m_ioContext;
			CEventHandler& m_ceventHandler;

			bool m_accepting;
		};
	}
}