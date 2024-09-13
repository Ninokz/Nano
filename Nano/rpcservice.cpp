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
			ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcService") << "invoke: "<< methodName << std::endl;
			it->second->invoke(request, done);
		}

		void RpcService::callProcedureNotify(std::string methodName, 
			Json::Value& request)
		{
			auto it = m_procedureNotfiy.find(methodName);
			if (it == m_procedureNotfiy.end()) 
				throw RpcProtoException(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound);
			ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcService") << "invoke: " << methodName << std::endl;
			it->second->invoke(request);
		}

		bool RpcService::hasProcedureReturn(std::string methodName)
		{
			return this->m_procedureReturn.find(methodName) != this->m_procedureReturn.end();
		}

		bool RpcService::hasProcedureNotify(std::string methodName)
		{
			return this->m_procedureNotfiy.find(methodName) != this->m_procedureNotfiy.end();
		}
	}
}