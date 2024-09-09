#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

#include <json/json.h>

namespace Nano {
	namespace JrpcProto {
		class JsonRpcRequest {
		public:
			typedef std::shared_ptr<JsonRpcRequest> Ptr;
		public:
			JsonRpcRequest() = default;

			JsonRpcRequest(const std::string& jsonrpcVersion, const std::string& methodName, const Json::Value& parameters, int requestId)
				: m_ver(jsonrpcVersion), m_method(methodName), m_params(parameters), m_id(requestId) {}
			static bool fromJson(const std::string& jsonStr, JsonRpcRequest& request);
		public:
			std::string m_ver;
			std::string m_method;
			Json::Value m_params;
			int m_id;
		};

        class JsonRpcError {
		public:
			enum class JsonRpcErrorCode {
				ParseError = -32700,
				InvalidRequest = -32600,
				MethodNotFound = -32601,
				InvalidParams = -32602,
				InternalError = -32603
			};
			typedef std::shared_ptr<JsonRpcError> Ptr;
        public:
            JsonRpcError(JsonRpcErrorCode errorCode)
				: code(errorCode), message(getErrorMessage(errorCode)) {}

            JsonRpcError(JsonRpcErrorCode errorCode, const std::string& errorMsg)
				: code(errorCode), message(errorMsg) {}

			Json::Value toJson() const;

			static std::string getErrorMessage(JsonRpcErrorCode code);
		public:
			JsonRpcErrorCode code;
			std::string message;
        };

        class JsonRpcResponse {
        public:
            std::string jsonrpc;
            Json::Value result;
            JsonRpcError* error = nullptr;
            int m_id;

            // 构造函数：成功响应
            JsonRpcResponse(const std::string& jsonrpcVersion, int requestId, const Json::Value& resultValue)
                : jsonrpc(jsonrpcVersion), m_id(requestId), result(resultValue), error(nullptr) {}

            // 构造函数：错误响应
            JsonRpcResponse(const std::string& jsonrpcVersion, int requestId, JsonRpcError::JsonRpcErrorCode errorCode)
                : jsonrpc(jsonrpcVersion), m_id(requestId), error(new JsonRpcError(errorCode)) {}

			~JsonRpcResponse();

			std::string toJson() const;
        };

	}
}