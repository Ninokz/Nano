#include "rpcserver.h"

namespace Nano {
	namespace Rpc {
		RpcServer::RpcServer(short port) : Communication::BaseServer(port), m_rpcService(std::make_unique<RpcService>())
		{

		}

		RpcServer::~RpcServer()
		{
		}

		void RpcServer::Init()
		{
			m_ceventHandler->AddDataReadyHandler(weak_from_this());
		}

		void RpcServer::addProcedureReturn(std::string methodName, RpcService::ProcedureReturnUniqPtr p)
		{
			m_rpcService->addProcedureReturn(std::move(methodName), std::move(p));
		}

		void RpcServer::addProcedureNotify(std::string methodName, RpcService::ProcedureNotifyUniqPtr p)
		{
			m_rpcService->addProcedureNotify(std::move(methodName), std::move(p));
		}

		void RpcServer::OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
			std::string requestJsonStr = TransferCode::Code::decode(packet->m_data, packet->m_size);
			bool generateResult = false;
			Nano::JrpcProto::JsonRpcRequest::Ptr request = Nano::JrpcProto::JsonRpcRequest::generate(requestJsonStr, &generateResult);
			ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcServer") << "requestJsonStr: " << requestJsonStr << std::endl;
			if (!generateResult) {
				handleParseError(sender);
			}
			else {
				if (request->isReturnCall()) {
					handleProcedureReturn(sender, request);
				}
				else if (request->isNotification()) {
					handleProcedureNotify(sender, request);
				}
			}
		}

		void RpcServer::handleProcedureReturn(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			if (this->m_rpcService->hasProcedureReturn(request->getMethod()))
			{
				Nano::Concurrency::StealThreadPool::GetInstance()->submit([this, sender, request]() {
					std::string method = request->getMethod();
					Json::Value reqJson = request->toJson();
					ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcServer") << "method: " << method << std::endl;

					this->m_rpcService->callProcedureReturn(method, reqJson,
						[this, sender](Json::Value response) {
							ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcServer") << "response: " << response.toStyledString() << std::endl;
							bool flag = false;
							JrpcProto::JsonRpcResponse::Ptr jrp = JrpcProto::JsonRpcResponse::generate(response, &flag);
							ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcServer") << "ready to send: " << jrp->toJsonStr() << std::endl;
							if (flag) {
								std::string responseStr = jrp->toJsonStr();
								char* buffer = nullptr;
								int len = 0;
								ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcServer") << "send: " << responseStr << std::endl;
								/*if (TransferCode::Code::encode(responseStr, &buffer, &len))
									sender->Send(buffer, len);
								else
									handleParseError(sender);*/
								delete[] buffer;
								buffer = nullptr;
							}
							else
							{
								handleParseError(sender);
							}
					});
				});
			}
			else
			{
				handleMethodNotFound(sender, request);
			}
		}

		void RpcServer::handleProcedureNotify(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			if (this->m_rpcService->hasProcedureNotify(request->getMethod()))
			{
				auto stealthreadPool = Nano::Concurrency::StealThreadPool::GetInstance();
				stealthreadPool->submit([this, sender, request]() mutable {
					std::string method = request->getMethod();
					Json::Value reqJson = request->toJson();
					this->m_rpcService->callProcedureNotify(method, reqJson);
				});
			}
			else
			{
				handleMethodNotFound(sender, request);
			}
		}

		void RpcServer::handleParseError(std::shared_ptr<Communication::Session> sender)
		{
			/// 暂时直接抛出异常
			throw RpcProtoException(JrpcProto::JsonRpcError::JsonRpcErrorCode::ParseError);
		}

		void RpcServer::handleMethodNotFound(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			/// 暂时直接抛出异常
			throw RpcProtoException(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound);
		}
	}
}