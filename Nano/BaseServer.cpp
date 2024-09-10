#include "BaseServer.h"

namespace Nano {
	namespace Communication {
		BaseServer::BaseServer(short port) :
			m_port(port),
			m_running(false),
			m_ioc(),
			m_ceventHandler(std::make_shared<CEventHandler>()),
			m_sessionManager(std::make_shared<SessionManager>(Const::MAS_SESSION))
		{
			m_ceventHandler->AddConnectHandler(std::static_pointer_cast<IConnectEventHandler>(m_sessionManager));
			m_ceventHandler->AddCloseHandler(std::static_pointer_cast<ICloseEventHandler>(m_sessionManager));
			m_acceptor = std::make_unique<ConnectionAcceptor>(m_ioc, port, *m_ceventHandler);
		}

		BaseServer::~BaseServer()
		{
			try {
				Stop();
			}
			catch (std::exception& e)
			{
				std::cerr << "Exception: " << e.what() << std::endl;
			}
		}

		void BaseServer::Start()
		{
			try {
				boost::asio::signal_set signals(m_ioc, SIGINT, SIGTERM);
				signals.async_wait([this](auto, auto) {
					m_running = false;
					m_ioc.stop();
					m_acceptor->StopAccept();
					});

				m_listenThread = std::thread([this]() {
					m_running = true;
					m_acceptor->StartAccept();
					m_ioc.run();
					});
				ASYNC_LOG_INFO(ASYNC_LOG_NAME("SERVER_STD_LOGGER"), "BaseServer") << "Server started on port: " << m_port;
				m_listenThread.join();
			}
			catch (std::exception& e)
			{
				std::cerr << "Exception: " << e.what() << std::endl;
			}
		}
		void BaseServer::Stop()
		{
			if (m_running)
			{
				m_ioc.stop();
				m_listenThread.join();
			}
			ASYNC_LOG_INFO(ASYNC_LOG_NAME("SERVER_STD_LOGGER"), "BaseServer") << "Server stop";
		}
	}
}