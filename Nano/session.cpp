#include "session.h"
#include "ceventHandler.h"

namespace Nano {
	namespace Communication {
		Session::Session(boost::asio::io_context& ioContext, CEventHandler& ceventHandler) :
			m_socket(ioContext),
			m_ceventHandler(ceventHandler),
			m_uid(boost::uuids::to_string(boost::uuids::random_generator()())),
			m_recvHead(std::make_shared<RecvPacket>(Const::PACKET_HEAD_SIZE))
		{
		}

		Session::~Session()
		{
			ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << "Session destroyed: " << this->m_uid << std::endl;
		}

		void Session::Start()
		{
			try {
				boost::asio::async_read(
					m_socket,
					boost::asio::buffer(m_recvHead->m_data, Const::PACKET_HEAD_SIZE),
					boost::bind(&Session::HandleHeadRead, shared_from_this(),
						std::placeholders::_1, std::placeholders::_2)
				);
				m_ceventHandler.OnConnected(shared_from_this());
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				this->Close();
			}
		}

		void Session::Close()
		{
			m_ceventHandler.OnClosed(shared_from_this());
			m_socket.close();
		}

		void Session::Send(const char* data, int dataLength)
		{
			try {
				bool queuePending = false;
				std::lock_guard<std::mutex> lock(m_sendMutex);
				if (this->m_sendQueue.size() >= Const::MAX_SESSION_SEND_QUEUE_PENDING_SIZE) {
					/// queue is full
					ASYNC_LOG_WARN(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << "Send queue is full, session: " << this->m_uid << std::endl;
					return;
				}
				if (!this->m_sendQueue.empty()) {
					queuePending = true;
				}
				this->m_sendQueue.push(std::make_shared<SendPacket>(data, dataLength));
				if (queuePending)
				{
					return;
				}
				auto& nxtPacket = this->m_sendQueue.front();
				ASYNC_LOG_DEBUG(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << "send" << std::endl;
				boost::asio::async_write(
					this->m_socket,
					boost::asio::buffer(nxtPacket->m_data, nxtPacket->m_size),
					boost::bind(&Session::HandleWrite, shared_from_this(), std::placeholders::_1)
				);
			}
			catch (std::exception& e) {
				//std::cerr << e.what() << std::endl;
				ASYNC_LOG_ERROR(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << e.what() << std::endl;
				this->Close();
			}
		}

		void Session::HandleHeadRead(const boost::system::error_code& ec, size_t bytes_tramsferred)
		{
			try {
				if (!ec)
				{
					if (bytes_tramsferred < Const::PACKET_HEAD_SIZE || bytes_tramsferred > Const::PACKET_HEAD_SIZE)
					{
						this->Close();
						return;
					}
					unsigned short dataSize = 0;
					memcpy(&dataSize, m_recvHead->m_data, Const::PACKET_HEAD_SIZE);
					dataSize = boost::asio::detail::socket_ops::network_to_host_short(dataSize);

					if (dataSize > Const::PACKET_MAX_SIZE)
					{
						this->Close();
						return;
					}
					m_recvPacket = std::make_shared<RecvPacket>(dataSize);

					boost::asio::async_read(
						m_socket,
						boost::asio::buffer(m_recvPacket->m_data, m_recvPacket->m_size),
						boost::bind(&Session::HandleBodyRead, shared_from_this(),
							std::placeholders::_1, std::placeholders::_2)
					);
				}
				else {
					ASYNC_LOG_WARN(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << "HandleHeadRead error: " << ec.message() << std::endl;
					this->Close();
				}
			}
			catch (std::exception& e) {
				//std::cerr << e.what() << std::endl;
				ASYNC_LOG_ERROR(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << e.what() << std::endl << std::endl;
				this->Close();
			}
		}

		void Session::HandleBodyRead(const boost::system::error_code& ec, size_t bytes_tramsferred)
		{
			try {
				if (!ec)
				{
					/// copy m_recvPacket
					auto newPacket = std::make_shared<RecvPacket>(*m_recvPacket);
					///// test
					//std::cout << newPacket->ToString() << std::endl;
					/// notify data ready
					this->m_ceventHandler.OnDataReady(shared_from_this(), newPacket);
					/// read next packet
					this->m_recvPacket->Clear();
					boost::asio::async_read(
						m_socket,
						boost::asio::buffer(m_recvHead->m_data, Const::PACKET_HEAD_SIZE),
						boost::bind(&Session::HandleHeadRead, shared_from_this(),
							std::placeholders::_1, std::placeholders::_2)
					);
				}
				else
				{
					ASYNC_LOG_WARN(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << "HandleBodyRead error: " << ec.message() << std::endl;
					this->Close();
				}
			}
			catch (std::exception& e) {
				//std::cerr << e.what() << std::endl;
				ASYNC_LOG_ERROR(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << e.what() << std::endl;
				this->Close();
			}
		}

		void Session::HandleWrite(const boost::system::error_code& ec)
		{
			try {
				if (!ec) {
					std::lock_guard<std::mutex> lock(m_sendMutex);
					this->m_sendQueue.pop();
					if (!this->m_sendQueue.empty())
					{
						auto& nxtPacket = this->m_sendQueue.front();
						boost::asio::async_write(
							this->m_socket,
							boost::asio::buffer(nxtPacket->m_data, nxtPacket->m_size),
							boost::bind(&Session::HandleWrite, shared_from_this(), std::placeholders::_1)
						);
					}
				}
				else {
					ASYNC_LOG_WARN(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << "HandleWrite error: " << ec.message() << std::endl;
					Close();
				}
			}
			catch (std::exception& e) {
				//std::cerr << e.what() << std::endl;
				ASYNC_LOG_ERROR(ASYNC_LOG_NAME("STD_LOGGER"), "Session") << e.what() << std::endl;
				this->Close();
			}
		}
	}
}