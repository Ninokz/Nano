#include "rpcserver.h"

namespace Nano {
	namespace Rpc {
		RpcServer::RpcServer(short port) : 
			Communication::BaseServer(port)
		{

		}

		RpcServer::~RpcServer()
		{
		}

		void RpcServer::Init()
		{
			m_ceventHandler->AddDataReadyHandler(weak_from_this());
		}

		void RpcServer::addService(std::string serviceName, RpcService::Ptr service)
		{
			m_services.emplace(std::move(serviceName), std::move(service));
		}

		void RpcServer::delService(std::string serviceName)
		{
			m_services.erase(serviceName);
		}

		void RpcServer::OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
			std::cout << packet->ToString() << std::endl;
		}

		void RpcServer::handlePacket(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
		}

		void RpcServer::handleProcedureReturn(std::shared_ptr<Communication::Session> sender, Json::Value& request)
		{
		}

		void RpcServer::handleProcedureNotify(std::shared_ptr<Communication::Session> sender, Json::Value& request)
		{
		}
	}
}