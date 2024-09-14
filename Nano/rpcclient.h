#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "ceventhandler.h"
#include "session.h"
#include "packet.h"

#include "BaseClient.h"
#include "RpcService.h"
#include "RpcProcedure.h"

#include "stealThreadPool.h"
#include "functionWrapper.h"

#include "jrpcproto.h"
#include "code.h"
#include "rpcexception.h"

#include "Log.h"

namespace Nano {
	namespace Rpc {
		class RpcClient : public Communication::BaseClient, public Communication::IDataReadyEventHandler,
			public std::enable_shared_from_this<RpcClient>
		{
		public:
			class RpcCallRecord
			{
			public:
				typedef std::shared_ptr<RpcCallRecord> Ptr;
				RpcCallRecord() = delete;
				RpcCallRecord(JrpcProto::JsonRpcRequest::Ptr request) : request(request), response(nullptr), timestamp(std::time(nullptr)) {}
				~RpcCallRecord() = default;
			public:
				time_t timestamp;
				JrpcProto::JsonRpcRequest::Ptr request;
				JrpcProto::JsonRpcResponse::Ptr response;
			};
			typedef std::shared_ptr<RpcClient> Ptr;
			typedef std::unordered_map<std::string, std::pair<RpcCallRecord::Ptr, RpcDoneCallback>> RpcCallRecordMap;
		public:
			RpcClient();

			virtual ~RpcClient();
			void Init();
			bool callReturnProcedure(JrpcProto::JsonRpcRequest::Ptr request, const RpcDoneCallback callback);
			bool callNotifyProcedure(JrpcProto::JsonRpcRequest::Ptr request);
			RpcCallRecord::Ptr getReturnCallRecord(const std::string& id);
		private:
			void OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet) override;
		private:
			RpcCallRecordMap m_callRecords;
		};
	}
}