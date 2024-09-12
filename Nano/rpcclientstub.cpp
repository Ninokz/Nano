#include "rpcclientstub.h"

namespace Nano {
	namespace Rpc {
		RpcClientStub::RpcClientStub()
		{
		}

		RpcClientStub::~RpcClientStub()
		{
		}

		void RpcClientStub::rpcReturnCall(std::string ip, short port, std::string id, std::string methodName,
			Json::Value params,
			const RpcResponseCallback callback, int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = Nano::JrpcProto::JrpcRequestGenerator::generate(methodName, id, params);
			m_rpcClient = std::make_shared<RpcClient>();
			m_rpcClient->Connect(ip, port);
			this->m_rpcClient->callReturnProcedure(request, callback);
			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
			m_rpcClient->Disconnect();
			m_rpcClient.reset();
		}

		void RpcClientStub::asyncRpcReturnCall(std::string ip, short port, std::string id, std::string methodName, 
			Json::Value params,
			const RpcResponseCallback callback, int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = Nano::JrpcProto::JrpcRequestGenerator::generate(methodName, id, params);
			m_rpcClient = std::make_shared<RpcClient>();
			m_rpcClient->Connect(ip, port);

			Nano::Concurrency::ThreadPool::GetInstance()->Commit([this, request, callback, milliseconds_timeout]() {
				this->m_rpcClient->callReturnProcedure(request, callback);
				std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
				m_rpcClient->Disconnect();
				m_rpcClient.reset();
			});
		}

		void RpcClientStub::rpcNotifyCall(std::string ip, short port, std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap)
		{
			Json::Value params;
			for (auto& it : paramsNameTypesMap)
			{
				params[it.first] = it.second;
			}
			Nano::JrpcProto::JsonRpcRequest::Ptr request = Nano::JrpcProto::JrpcRequestGenerator::generate(methodName, params);
			m_rpcClient = std::make_shared<RpcClient>();
			m_rpcClient->Connect(ip, port);
			this->m_rpcClient->callNotifyProcedure(request);
			m_rpcClient->Disconnect();
			m_rpcClient.reset();
		}
	}
}