#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <ctime>

#include "jrpcproto.h"
#include "code.h"

namespace Nano {
	namespace Rpc {
		struct RpcCallRecord
		{
			typedef std::shared_ptr<RpcCallRecord> Ptr;
			RpcCallRecord() = delete;
			RpcCallRecord(JrpcProto::JsonRpcRequest::Ptr request) : request(request), response(nullptr)
			{
				timestamp = std::time(nullptr);
			}
			~RpcCallRecord() = default;

			time_t timestamp;
			JrpcProto::JsonRpcRequest::Ptr request;
			JrpcProto::JsonRpcResponse::Ptr response;
		};
	}
}