#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "ceventhandler.h"
#include "session.h"
#include "packet.h"

#include "BaseServer.h"
#include "RpcService.h"
#include "RpcProcedure.h"

#include "stealThreadPool.h"
#include "parrallenThreadPool.h"
#include "functionWrapper.h"

#include "jrpcproto.h"
#include "rpcexception.h"
#include "code.h"

#include "Log.h"

namespace Nano {
	namespace Rpc {
		class RpcServer : public Communication::BaseServer, public Communication::IDataReadyEventHandler, 
			public std::enable_shared_from_this<RpcServer>
		{
		public:
			typedef std::shared_ptr<RpcServer> Ptr;

			RpcServer(short port);
			virtual ~RpcServer();

			static RpcServer::Ptr Create(short port) {
				return std::make_shared<RpcServer>(port);
			}

			void Init();

			void addProcedureReturn(std::string methodName, RpcService::ProcedureReturnUniqPtr p);

			void addProcedureNotify(std::string methodName, RpcService::ProcedureNotifyUniqPtr p);

		private:
			void OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet) override;
			void handleProcedureReturn(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request);
			void handleProcedureNotify(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request);

			void handleJsonRpcErrorException(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcError::JsonRpcErrorCode code);
		private:
			RpcService::Ptr m_rpcService;
		};
	}
}