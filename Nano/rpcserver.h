#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "ceventhandler.h"
#include "session.h"
#include "packet.h"

#include "BaseServer.h"
#include "RpcService.h"
#include "RpcProcedure.h"

#include "stealThreadPool.h"
#include "functionWrapper.h"

#include "Log.h"

namespace Nano {
	namespace Rpc {
		class RpcServer : public Communication::BaseServer, public Communication::IDataReadyEventHandler, 
			public std::enable_shared_from_this<RpcServer>
		{
		public:
			typedef std::shared_ptr<RpcServer> Ptr;

			RpcServer(short port);
			virtual ~RpcServer();

			static RpcServer::Ptr Create(short port) {
				return std::make_shared<RpcServer>(port);
			}
			void Init();
			void addService(std::string serviceName, RpcService::Ptr service);
			void delService(std::string serviceName);
		private:
			void OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet) override;
			void handlePacket(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet);
			void handleProcedureReturn(std::shared_ptr<Communication::Session> sender, Json::Value& request);
			void handleProcedureNotify(std::shared_ptr<Communication::Session> sender, Json::Value& request);
		private:
			std::unordered_map<std::string, RpcService::Ptr> m_services;
		};
	}
}