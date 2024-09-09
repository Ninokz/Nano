#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <queue>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "const.h"
#include "packet.h"
#include "nocopyable.h"

namespace Nano {
	namespace Communication {

		class CEventHandler;
		class Session : public std::enable_shared_from_this<Session>, public Noncopyable 
		{
		public:
			typedef std::shared_ptr<Session> Ptr;
		public:
			Session(boost::asio::io_context& ioContext, CEventHandler& ceventHandler);
			virtual ~Session();

			void Start();
			void Close();
			void Send(const char* data, int dataLength);

			std::string getUid() const {
				return this->m_uid;
			}

			boost::asio::ip::tcp::socket& getSocket() {
				return m_socket;
			}
		private:
			void HandleHeadRead(const boost::system::error_code& ec, size_t bytes_tramsferred);
			void HandleBodyRead(const boost::system::error_code& ec, size_t bytes_tramsferred);
			void HandleWrite(const boost::system::error_code& ec);
		private:
			std::string m_uid;
			boost::asio::ip::tcp::socket m_socket;
			std::shared_ptr<RecvPacket> m_recvHead;
			std::shared_ptr<RecvPacket> m_recvPacket;
			std::mutex m_sendMutex;
			std::queue<std::shared_ptr<SendPacket>> m_sendQueue;

			CEventHandler& m_ceventHandler;
		};
	}
}
