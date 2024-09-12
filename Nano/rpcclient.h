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
#include "rpccallrecord.h"

#include "Log.h"

namespace Nano {
	namespace Rpc {
		typedef std::unordered_map<std::string, RpcCallRecord::Ptr> RpcCallRecordMap;
		class RpcClient : public Communication::BaseClient, public Communication::IDataReadyEventHandler,
			public std::enable_shared_from_this<RpcClient>
		{
		public:
			typedef std::shared_ptr<RpcClient> Ptr;
			
		public:
			typedef std::function<void(Json::Value& response)> RpcResponseCallback;

			RpcClient();
			virtual ~RpcClient();

			bool connect(const std::string& ip, short port);
			void disconnect();

			bool callReturnProcedure(JrpcProto::JsonRpcRequest::Ptr request);
			bool callNotifyProcedure(JrpcProto::JsonRpcRequest::Ptr request);

			RpcCallRecord::Ptr getCallRecord(std::string id);
			void removeCallRecord(std::string id);
		private:
			void OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet) override;
		private:
			std::mutex m_mutex;
			RpcCallRecordMap m_callRecords;
		};
	}
}