#pragma once
#include <string>
#include <exception>

#include "jrpcproto.h"

namespace Nano {
	namespace Rpc {
        class RpcException : public std::exception
        {
        public:
			explicit RpcException(int code, std::string detail) : m_code(code), m_detail(detail) {}
			const char* what() const noexcept override { return m_detail.c_str(); }
            int err() const noexcept { return m_code; }
			const std::string detail() const noexcept { return m_detail; }
        private:
			int m_code;
            const std::string m_detail;
        };

		class RpcProtoException : public RpcException
		{
		public:
			explicit RpcProtoException(JrpcProto::JsonRpcError::JsonRpcErrorCode code) : 
				RpcException(JrpcProto::JsonRpcError::toInt(code), JrpcProto::JsonRpcError::getErrorMessage(code)) {}
		};
	}
}