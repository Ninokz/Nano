#include "rpcclientstub.h"

namespace Nano {
	namespace Rpc {
		void RpcClientStub::rpcReturnCall(std::string ip, short port, std::string id, std::string methodName,
			std::unordered_map<std::string, Json::Value> params,
			const RpcDoneCallback callback, int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = Nano::JrpcProto::JsonRpcRequest::generateReturnCallRequest("2.0", methodName, id, params);
			m_rpcClient = std::make_shared<RpcClient>();
			m_rpcClient->Connect(ip, port);
			this->m_rpcClient->callReturnProcedure(request, callback);
			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
			m_rpcClient->Disconnect();
			m_rpcClient.reset();
		}

		void RpcClientStub::asyncRpcReturnCall(std::string ip, short port, std::string id, std::string methodName,
			std::unordered_map<std::string, Json::Value> params,
			const RpcDoneCallback callback, int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = Nano::JrpcProto::JsonRpcRequest::generateReturnCallRequest("2.0", methodName, id, params);
			m_rpcClient = std::make_shared<RpcClient>();
			m_rpcClient->Connect(ip, port);

			Nano::Concurrency::ThreadPool::GetInstance()->Commit([this, request, callback, milliseconds_timeout]() {
				this->m_rpcClient->callReturnProcedure(request, callback);
				std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
				m_rpcClient->Disconnect();
				m_rpcClient.reset();
			});
		}

		void RpcClientStub::rpcNotifyCall(std::string ip, short port, std::string methodName, std::unordered_map<std::string, Json::Value> params)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = Nano::JrpcProto::JsonRpcRequest::generateNotifyCallRequest("2.0", methodName, params);
			m_rpcClient = std::make_shared<RpcClient>();
			m_rpcClient->Connect(ip, port);
			this->m_rpcClient->callNotifyProcedure(request);
			m_rpcClient->Disconnect();
			m_rpcClient.reset();
		}

		void RpcClientStub::asyncRpcNotifyCall(std::string ip, short port, std::string methodName, std::unordered_map<std::string, Json::Value> params)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = Nano::JrpcProto::JsonRpcRequest::generateNotifyCallRequest("2.0", methodName, params);
			m_rpcClient = std::make_shared<RpcClient>();
			m_rpcClient->Connect(ip, port);

			Nano::Concurrency::ThreadPool::GetInstance()->Commit([this, request]() {
				this->m_rpcClient->callNotifyProcedure(request);
				m_rpcClient->Disconnect();
				m_rpcClient.reset();
			});
		}
	}
}