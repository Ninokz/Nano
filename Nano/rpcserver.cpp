#include "rpcserver.h"

namespace Nano {
	namespace Rpc {
		RpcServer::RpcServer(short port) : Communication::BaseServer(port)
		{

		}

		RpcServer::~RpcServer()
		{
		}

		RpcServer::Ptr RpcServer::Create(short port)
		{
			return std::make_shared<RpcServer>(port);
		}

		void RpcServer::Init()
		{
			m_ceventHandler->AddDataReadyHandler(weak_from_this());
		}

		void RpcServer::OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
			if (sender && packet)
			{
				std::string requestJsonStr = TransferCode::Code::decode(packet->m_data, packet->m_size);
				bool generateResult = false;
				JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JrpcRequestGenerator::generate(requestJsonStr, &generateResult);
				if (generateResult) {

				}
				else
				{

				}
			}
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