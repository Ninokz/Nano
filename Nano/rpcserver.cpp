#include "rpcserver.h"

namespace Nano {
	namespace Rpc {
		RpcServer::RpcServer(short port) : Communication::BaseServer(port), m_rpcService(std::make_unique<RpcService>())
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
					ASYNC_LOG_DEBUG(ASYNC_LOG_NAME("STD_LOGGER"), "RpcServer") << packet->ToString();
					if (request->isReturnRequest()) {
						handleProcedureReturn(sender, request);
					}
					else if (request->isNotification()) {
						handleProcedureNotify(sender, request);
					}
				}
				else
				{
					std::cerr << packet->ToString() << std::endl;
				}
			}
		}

		void RpcServer::handleProcedureReturn(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			if (this->m_rpcService->hasProcedureReturn(request->m_method))
			{
				auto stealthreadPool = Nano::Concurrency::StealThreadPool::GetInstance();
				auto future = stealthreadPool->submit([this, sender, request]() mutable {
					this->m_rpcService->callProcedureReturn(request->m_method, request->m_params,
					[this, sender, request](Json::Value& response) {
							bool flag = false;
							JrpcProto::JsonRpcResponse::Ptr jsresponse = JrpcProto::JrpcResponseParser::parse(response, &flag);
							if (flag)
							{
								/// 解析成功
								std::string responseJsonStr = jsresponse->toJsonStr();
								char* buffer = nullptr;
								int len = 0;
								if (TransferCode::Code::encode(responseJsonStr, &buffer, &len))
								{
									sender->Send(buffer, len);
									delete buffer;
									buffer = nullptr;
								}
								else
									std::cerr << "handleProcedureReturn: encode failed" << std::endl;
								if (buffer)
								{
									delete buffer;
									buffer = nullptr;
								}
							}
							else
							{
								/// 解析失败
								JrpcProto::JsonRpcResponse internalErrorResponse("2.0", request->m_id, Nano::JrpcProto::JsonRpcError::JsonRpcErrorCode::InternalError);
								std::string responseJsonStr = internalErrorResponse.toJsonStr();
								char* buffer = nullptr;
								int len = 0;
								if (TransferCode::Code::encode(responseJsonStr, &buffer, &len))
								{
									sender->Send(buffer, len);
									delete buffer;
									buffer = nullptr;
								}
								else
									std::cerr << "handleProcedureReturn: encode failed" << std::endl;
								if (buffer)
								{
									delete buffer;
									buffer = nullptr;
								}
							}
						});
					});
				future.get();
			}
			else
			{
				handleMethodNotFound(sender, request);
			}
		}

		void RpcServer::handleProcedureNotify(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			if (this->m_rpcService->hasProcedureNotify(request->m_method))
			{
				auto stealthreadPool = Nano::Concurrency::StealThreadPool::GetInstance();
				stealthreadPool->submit([this, sender, request]() mutable {
					this->m_rpcService->callProcedureNotify(request->m_method, request->m_params);
					});
			}
			else
			{
				handleMethodNotFound(sender, request);
			}
		}

		void RpcServer::handleMethodNotFound(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			JrpcProto::JsonRpcResponse notFoundResponse("2.0", request->m_id, Nano::JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound);
			std::string responseJsonStr = notFoundResponse.toJsonStr();
			char* buffer = nullptr;
			int len = 0;
			if (TransferCode::Code::encode(responseJsonStr, &buffer, &len))
			{
				sender->Send(buffer, len);
				delete buffer;
				buffer = nullptr;
			}
			else
				std::cerr << "handleMethodNotFound: encode failed" << std::endl;
			if (buffer)
			{
				delete buffer;
				buffer = nullptr;
			}
		}
	}
}