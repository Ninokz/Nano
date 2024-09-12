#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include <json/json.h>

#include "nocopyable.h"
#include "rpcclient.h"
#include "threadpool.h"

namespace Nano {
	namespace Rpc {
		class RpcClientStub : public Noncopyable
		{
		public:
			typedef std::shared_ptr<RpcClientStub> Ptr;
			RpcClientStub();
			~RpcClientStub();

			void rpcReturnCall(std::string ip, short port, std::string id, std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap, const RpcResponseCallback callback, int milliseconds_timeout);
			void asyncRpcReturnCall(std::string ip, short port, std::string id, std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap, const RpcResponseCallback callback, int milliseconds_timeout);
			void rpcNotifyCall(std::string ip, short port, std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap);

			RpcCallRecord::Ptr getReturnCallResult(std::string id) {
				return m_rpcClient->getReturnCallRecord(id);
			}
		private:
			RpcClient::Ptr m_rpcClient;
		};
	}
}