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
			try {
				std::string requestJsonStr = TransferCode::Code::decode(packet->m_data, packet->m_size);
				bool generateResult = false;
				Nano::JrpcProto::JsonRpcRequest::Ptr request = Nano::JrpcProto::JsonRpcRequest::generate(requestJsonStr, &generateResult);
				if (!generateResult) {
					throw RpcProtoException(JrpcProto::JsonRpcError::JsonRpcErrorCode::ParseError);
				}
				else {
					ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcServer") << "request is: " << request->toJsonStr() << std::endl;
					if (request->isReturnCall()) {
						handleProcedureReturn(sender, request);
					}
					else if (request->isNotification()) {
						handleProcedureNotify(sender, request);
					}
					else {
						throw RpcProtoException(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidRequest);
					}
				}
			}
			catch (RpcProtoException& e) {
				ASYNC_LOG_WARN(ASYNC_LOG_NAME("STD_LOGGER"), "RpcServer") << "RpcProtoException: " << e.err() << " " << e.detail() << std::endl;
				Nano::JrpcProto::JsonRpcError::JsonRpcErrorCode code = Nano::JrpcProto::JsonRpcError::fromInt(e.err());
				handleJsonRpcErrorException(sender, code);
			}
		}

		void RpcServer::handleProcedureReturn(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			Nano::Concurrency::StealThreadPool::GetInstance()->submit([this, sender, request]() {
				std::string method = request->getMethod();
				Json::Value reqJson = request->toJson();
				this->m_rpcService->callProcedureReturn(method, reqJson,
					[this, sender](Json::Value response) {
						bool flag = false;
						JrpcProto::JsonRpcResponse::Ptr jrp = JrpcProto::JsonRpcResponse::generate(response, &flag);
						if (flag) {
							ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcServer") << "response is: " << response.toStyledString() << std::endl;
							std::string responseStr = jrp->toJsonStr();
							char* buffer = nullptr;
							int len = 0;
							if (TransferCode::Code::encode(responseStr, &buffer, &len))
							{
								sender->Send(buffer, len);
							}
							else
								throw std::exception("encode error");
							delete[] buffer;
							buffer = nullptr;
						}
						else
						{
							throw RpcProtoException(JrpcProto::JsonRpcError::JsonRpcErrorCode::ParseError);
						}
					});
				});
		}

		void RpcServer::handleProcedureNotify(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			auto stealthreadPool = Nano::Concurrency::StealThreadPool::GetInstance();
			stealthreadPool->submit([this, sender, request]() mutable {
				std::string method = request->getMethod();
				Json::Value reqJson = request->toJson();
				this->m_rpcService->callProcedureNotify(method, reqJson);
				});
		}

		void RpcServer::handleJsonRpcErrorException(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcError::JsonRpcErrorCode code)
		{
			JrpcProto::JsonRpcResponse::Ptr errorReponse = JrpcProto::JsonRpcResponse::generate("2.0", code);
			std::string responseStr = errorReponse->toJsonStr();
			char* buffer = nullptr;
			int len = 0;
			if (TransferCode::Code::encode(responseStr, &buffer, &len))
			{
				sender->Send(buffer, len);
				delete[] buffer;
				buffer = nullptr;
			}
			else
				throw std::exception("encode error");
		}
	}
}