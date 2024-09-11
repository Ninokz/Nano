#include "rpcservice.h"

namespace Nano {
	namespace Rpc {
		void RpcService::callProcedureReturn(std::string methodName, 
			Json::Value& request, 
			const RpcDoneCallback& done)
		{
			auto it = m_procedureReturn.find(methodName);
			if (it == m_procedureReturn.end()) {
				int code = JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound);
				const char* detail = JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound).c_str();
				throw RpcException(code, detail);
			}
			it->second->invoke(request, done);
		}

		void RpcService::callProcedureNotify(std::string methodName, 
			Json::Value& request)
		{
			auto it = m_procedureNotfiy.find(methodName);
			if (it == m_procedureNotfiy.end()) {
				int code = JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound);
				const char* detail = JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound).c_str();
				throw RpcException(code, detail);
			}
			it->second->invoke(request);
		}
	}
}