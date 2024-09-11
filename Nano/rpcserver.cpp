#include "rpcserver.h"

namespace Nano {
	namespace Rpc {
		RpcServer::RpcServer(short port) : Communication::BaseServer(port)
		{
			
		}

		RpcServer::~RpcServer()
		{
		}

		void RpcServer::addService(std::string serviceName, RpcService::Ptr service)
		{
			m_services.emplace(std::move(serviceName), std::move(service));
		}

		void RpcServer::delService(std::string serviceName)
		{
			m_services.erase(serviceName);
		}

	}
}