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

		void RpcServer::addRpcDoneCallback(std::string methodName, RpcDoneCallback done)
		{
			if (m_rpcDoneCallback.find(methodName) == m_rpcDoneCallback.end())
			{
				m_rpcDoneCallback.emplace(std::move(methodName), std::move(done));
			}
			else
			{
				int code = JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound);
				std::string detail = JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound);
				throw RpcException(code, detail);
			}
		}

		void RpcServer::OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
			if (sender && packet)
			{
				std::string requestJsonStr = TransferCode::Code::decode(packet->m_data, packet->m_size);
				bool generateResult = false;
				JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JrpcRequestGenerator::generate(requestJsonStr, &generateResult);
				
				if (generateResult) {
					std::cout << packet->ToString() << std::endl;
					if (request->isReturnRequest()) {
						handleProcedureReturn(sender, request);
					}
					else if (request->isNotification()) {
						handleProcedureNotify(sender, request);
					}
				}
				else {
					std::cerr << packet->ToString() << std::endl;
				}
			}
		}

		void RpcServer::handleProcedureReturn(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			if (this->m_rpcService->hasProcedureReturn(request->m_method))
			{
				auto stealthreadPool = Nano::Concurrency::StealThreadPool::GetInstance();
				if (m_rpcDoneCallback.find(request->m_method) != m_rpcDoneCallback.end())
				{
					stealthreadPool->submit([this,request]() mutable {
						this->m_rpcService->callProcedureReturn(request->m_method, request->m_params, m_rpcDoneCallback[request->m_method]);
					});
				}
				else
				{
					/// TODO:此处应该输出警告日志，因为没有设置默认的回调函数
					/// 
					Nano::Rpc::RpcDoneCallback defalutDone = [](Json::Value val) {};
					stealthreadPool->submit([this, request, defalutDone]() mutable {
						this->m_rpcService->callProcedureReturn(request->m_method, request->m_params, defalutDone);
					});
				}
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
				if (m_rpcDoneCallback.find(request->m_method) != m_rpcDoneCallback.end())
				{
					stealthreadPool->submit([this, request]() mutable {
						this->m_rpcService->callProcedureNotify(request->m_method, request->m_params);
					});
				}
				else
				{
					/// TODO:此处应该输出警告日志，因为没有设置默认的回调函数
					/// 
					Nano::Rpc::RpcDoneCallback defalutDone = [](Json::Value val) {};
					stealthreadPool->submit([this, request, defalutDone]() mutable {
						this->m_rpcService->callProcedureNotify(request->m_method, request->m_params);
					});
				}
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
			{
				std::cerr << "handleMethodNotFound: encode failed" << std::endl;
			}
		}
	}
}