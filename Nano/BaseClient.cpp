#include "BaseClient.h"

namespace Nano {
	namespace Communication {
		BaseClient::BaseClient() :
			m_running(false),
			m_ioc(),
			m_ceventHandler(std::make_shared<CEventHandler>()),
			m_session(std::make_shared<Session>(m_ioc, *m_ceventHandler))
		{
		}

		BaseClient::~BaseClient()
		{
			Disconnect();
		}

		bool BaseClient::Connect(const std::string& ip, short port)
		{
			try {
				m_endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip), port);
				boost::system::error_code error = boost::asio::error::host_not_found;
				m_session->getSocket().connect(m_endpoint, error);
				if (error)
				{
					return false;
				}
				else
				{
					m_session->Start();
					m_running = true;
					m_clientThread = std::thread([this]() {
						m_ioc.run();
						});
					return true;
				}
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				return false;
			}
		}

		void BaseClient::Disconnect()
		{
			if (m_running)
			{
				m_running = false;
				m_ioc.stop();
				if (m_clientThread.joinable())
				{
					m_clientThread.join();
				}
			}
		}

		void BaseClient::Send(const char* data, int dataLength)
		{
			if (m_session)
			{
				m_session->Send(data, dataLength);
			}
		}
	}
}