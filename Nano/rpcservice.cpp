#include "rpcservice.h"

namespace Nano {
	namespace Rpc {
		void RpcService::callProcedureReturn(std::string methodName, 
			Json::Value& request, 
			const RpcDoneCallback& done)
		{
			auto it = m_procedureReturn.find(methodName);
			if (it == m_procedureReturn.end()) 
				throw RpcProtoException(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound);
			it->second->invoke(request, done);
		}

		void RpcService::callProcedureNotify(std::string methodName, 
			Json::Value& request)
		{
			auto it = m_procedureNotfiy.find(methodName);
			if (it == m_procedureNotfiy.end()) 
				throw RpcProtoException(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound);
			it->second->invoke(request);
		}
	}
}