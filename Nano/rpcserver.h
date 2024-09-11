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

namespace Nano {
	namespace Rpc {
		class RpcServer : public Communication::BaseServer
		{
		public:
			typedef std::shared_ptr<RpcServer> Ptr;
			RpcServer(short port);
			~RpcServer();

			void addService(std::string serviceName, RpcService::Ptr service);
			void delService(std::string serviceName);
		private:
			std::unordered_map<std::string, RpcService::Ptr> m_services;
			std::shared_ptr<Concurrency::StealThreadPool> m_threadPool;
		};
	}
}