#include "conacceptor.h"

namespace Nano {
	namespace Communication {
		ConnectionAcceptor::ConnectionAcceptor(boost::asio::io_context& ioc, short port,
			CEventHandler& ceventHandler):
			m_acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
			m_ioContext(ioc),
			m_ceventHandler(ceventHandler)
		{
		}

		ConnectionAcceptor::~ConnectionAcceptor()
		{
			m_acceptor.close();
		}

		void ConnectionAcceptor::StartAccept()
		{
			auto& ioc = IOServicePool::GetInstance()->getIOContext();
			std::shared_ptr<Session> newSession = std::make_shared<Session>(ioc, m_ceventHandler);
			m_acceptor.async_accept(newSession->getSocket(),
				boost::bind(&ConnectionAcceptor::HandleAccept, 
					this,newSession, boost::asio::placeholders::error));
		}

		void ConnectionAcceptor::HandleAccept(std::shared_ptr<Session> new_session, const boost::system::error_code& ec)
		{
			if (!ec)
			{
				new_session->Start();
			}
			else
			{
				new_session->Close();
			}
			StartAccept();
		}
	}
}