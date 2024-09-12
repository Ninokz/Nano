// JSON-RPC 2.0: A Remote Procedure Call (RPC) protocol using JSON
// https://www.jsonrpc.org/specification
//

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

			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, Json::Value parameters, std::string requestId)
				: m_ver(jsonrpcVersion), m_method(methodName), m_params(parameters), m_id(requestId) {}

			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, Json::Value parameters) :
				m_ver(jsonrpcVersion), m_method(methodName), m_params(parameters), m_id("") {}

			std::string toJsonStr() const;

			Json::Value toJson() const;

			bool isNotification() const {
				return m_id.empty();
			}

			bool isReturnRequest() const {
				return !m_id.empty();
			}
		public:
			std::string m_ver;
			std::string m_method;
			Json::Value m_params;
			std::string m_id;
		};

		class JrpcRequestGenerator {
		public:
			template <typename... Args>
			static JsonRpcRequest::Ptr generate(std::string method, std::string id, const Args&... args) {
				Json::Value params(Json::objectValue);
				addParams(params, args...);
				std::string ver = "2.0";
				JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>(ver, method, params, id);
				return request;
			}

			template <typename... Args>
			static JsonRpcRequest::Ptr generate(std::string method, const Args&... args) {
				Json::Value params(Json::objectValue);
				addParams(params, args...);
				std::string ver = "2.0";
				JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>(ver, method, params, "");
				return request;
			}

			static JsonRpcRequest::Ptr generate(const std::string& jsonStr, bool* flag);
		private:
			static void addParams(Json::Value& params) {}

			template <typename Key, typename Value, typename... Args>
			static void addParams(Json::Value& params, const Key& key, const Value& value, const Args&... args) {
				params[key] = value;
				addParams(params, args...);
			}
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
				: m_code(errorCode), m_message(getErrorMessage(errorCode)) {}

			Json::Value toJson() const;

			std::string	toJsonStr() const;

			static std::string getErrorMessage(JsonRpcErrorCode code);

			static JsonRpcErrorCode toErrorCode(const int code);

			static int toInt(JsonRpcErrorCode code);
		public:
			JsonRpcErrorCode m_code;
			std::string m_message;
		};

		class JsonRpcResponse {
		public:
			typedef std::shared_ptr<JsonRpcResponse> Ptr;
		public:
			JsonRpcResponse(std::string jsonrpcVersion, std::string requestId, Json::Value result) :
				jsonrpc(jsonrpcVersion), m_id(requestId), result(result) {}

			JsonRpcResponse( std::string jsonrpcVersion, std::string requestId, JsonRpcError::JsonRpcErrorCode errorCode) :
				jsonrpc(jsonrpcVersion), m_id(requestId), error(std::make_shared<JsonRpcError>(errorCode)) {}

			Json::Value toJson() const;

			std::string toJsonStr() const;
		public:
			std::string jsonrpc;
			std::string m_id;

			Json::Value result;
			JsonRpcError::Ptr error;
		};

		class JrpcResponseParser {
		public:
			static JsonRpcResponse::Ptr parse(const std::string& jsonStr, bool* flag);

			static JsonRpcResponse::Ptr parse(const Json::Value& root, bool* flag);
		private:
			static bool fieldsExist(const Json::Value& root);
		};
	}
}