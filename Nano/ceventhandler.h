#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "packet.h"
#include "session.h"

namespace Nano {
	namespace Communication {
		class ICloseEventHandler {
		public:
			virtual void OnClosed(std::shared_ptr<Session> sender) = 0;
		};

		class IConnectEventHandler
		{
		public:
			virtual void OnConnected(std::shared_ptr<Session> sender) = 0;
		};

		class IDataReadyEventHandler
		{
		public:
			virtual void OnDataReady(std::shared_ptr<Session> sender, std::shared_ptr<RecvPacket> packet) = 0;
		};

		class CEventHandler : public std::enable_shared_from_this<CEventHandler>
		{
		public:
			typedef std::shared_ptr<CEventHandler> Ptr;
		public:
			CEventHandler() = default;
			virtual ~CEventHandler() {
				this->m_connectHandlers.clear();
				this->m_closeHandlers.clear();
				this->m_dataReadyHandlers.clear();
			}

			void AddCloseHandler(std::shared_ptr<ICloseEventHandler> handler) {
				this->m_closeHandlers.push_back(handler);
			}
			void AddConnectHandler(std::shared_ptr<IConnectEventHandler> handler) {
				this->m_connectHandlers.push_back(handler);
			}
			void AddDataReadyHandler(std::shared_ptr<IDataReadyEventHandler> handler) {
				this->m_dataReadyHandlers.push_back(handler);
			}

			void OnClosed(std::shared_ptr<Session> sender) {
				std::lock_guard<std::mutex> lock(this->m_mutex);
				for (auto handler : this->m_closeHandlers) {
					handler->OnClosed(sender);
				}
			}

			void OnConnected(std::shared_ptr<Session> sender) {
				std::lock_guard<std::mutex> lock(this->m_mutex);
				for (auto handler : this->m_connectHandlers) {
					handler->OnConnected(sender);
				}
			}

			void OnDataReady(std::shared_ptr<Session> sender, std::shared_ptr<RecvPacket> packet) {
				std::lock_guard<std::mutex> lock(this->m_mutex);
				for (auto handler : this->m_dataReadyHandlers) {
					handler->OnDataReady(sender, packet);
				}
			}
		protected:
			std::vector<std::shared_ptr<ICloseEventHandler>> m_closeHandlers;
			std::vector<std::shared_ptr<IConnectEventHandler>> m_connectHandlers;
			std::vector<std::shared_ptr<IDataReadyEventHandler>> m_dataReadyHandlers;
			std::mutex m_mutex;
		};
	}
}