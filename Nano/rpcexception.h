#pragma once
#include <string>
#include <exception>

namespace Nano {
	namespace Rpc {
        class RpcException : public std::exception
        {
        public:
			explicit RpcException(int code, const char* detail) : m_code(code), m_detail(detail) {}
			const char* what() const noexcept override { return m_detail; }
            int err() const noexcept { return m_code; }
			const char* detail() const noexcept { return m_detail; }
        private:
			int m_code;
            const char* m_detail;
        };
	}
}