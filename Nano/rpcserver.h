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
#include "functionWrapper.h"

#include "jrpcproto.h"
#include "code.h"

#include "Log.h"

namespace Nano {
	namespace Rpc {
		class RpcServer : public Communication::BaseServer, public Communication::IDataReadyEventHandler,
			public std::enable_shared_from_this<RpcServer>
		{
		public:
			typedef std::shared_ptr<RpcServer> Ptr;
			static RpcServer::Ptr Create(short port);
			RpcServer(short port);
			virtual ~RpcServer();
			void Init();
			void addProcedureReturn(std::string methodName, RpcService::ProcedureReturnPtr p)
			{
				m_rpcService->addProcedureReturn(std::move(methodName), std::move(p));
			}
			void addProcedureNotify(std::string methodName, RpcService::ProcedureNotifyPtr p)
			{
				m_rpcService->addProcedureNotify(std::move(methodName), std::move(p));
			}
		private:
			void OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet) override;
			void handleProcedureReturn(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request);
			void handleProcedureNotify(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request);
			void handleMethodNotFound(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request);
		public:
			RpcService::Ptr m_rpcService;
		};
	}
}