#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <unordered_map>

#include <json/json.h>

namespace Nano {
	namespace JrpcProto {
		class JsonRpcRequest {
		public:
			typedef std::shared_ptr<JsonRpcRequest> Ptr;
			using ParameterName = std::string;
			using ParameterValue = Json::Value;
		public:
			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, Json::Value parametersNameWithValue, std::string requestId);

			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, Json::Value parametersNameWithValue);

			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, std::unordered_map<ParameterName, ParameterValue> kv, std::string requestId);

			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, std::unordered_map<ParameterName, ParameterValue> kv);

			JsonRpcRequest(const Json::Value& request);

			std::string toJsonStr() const;

			Json::Value toJson() const;

			std::string getMethod() const;

			std::string getId() const;

			Json::Value getParams() const;

			std::string getVersion() const;

			Json::Value getParam(const std::string& key) const;

			bool isNotification() const;

			bool isReturnCall() const;

			static JsonRpcRequest::Ptr generate(const std::string & jsonStr, bool* flag);

			static bool fieldsExist(const Json::Value& rpcRequestJson);

			static JsonRpcRequest::Ptr generateReturnCallRequest(const std::string& version, const std::string& method, const std::string id, std::unordered_map<std::string, Json::Value> params);

			static JsonRpcRequest::Ptr generateNotifyCallRequest(const std::string& version, const std::string& method, std::unordered_map<std::string, Json::Value> params);

			template <typename... Args>
			static JsonRpcRequest::Ptr generateReturnCallRequest(const std::string& version,const std::string& method, const std::string id, const Args&... args) {
				Json::Value params(Json::objectValue);
				addParams(params, args...);
				JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>(version, method, params, id);
				return request;
			}

			template <typename... Args>
			static JsonRpcRequest::Ptr generateNotifyCallRequest(const std::string& version, std::string method, const Args&... args) {
				Json::Value params(Json::objectValue);
				addParams(params, args...);
				JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>(version, method, params, "");
				return request;
			}
		private:
			static void addParams(Json::Value& params) {}

			template <typename Key, typename Value, typename... Args>
			static void addParams(Json::Value& params, const Key& key, const Value& value, const Args&... args) {
				params[key] = value;
				addParams(params, args...);
			}
		private:
			Json::Value m_rpcRequest;
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
			JsonRpcError(JsonRpcErrorCode errorCode);

			Json::Value toJson() const;

			std::string	toJsonStr() const;

			int getErrorCode() const;

			std::string getErrorMessage() const;

			static std::string getErrorMessage(JsonRpcErrorCode code);

			static int toInt(JsonRpcErrorCode code);
		private:
			Json::Value m_rpcError;
		};

		class JsonRpcResponse {
		public:
			typedef std::shared_ptr<JsonRpcResponse> Ptr;
		public:
			JsonRpcResponse(std::string jsonrpcVersion, std::string requestId, const Json::Value result);
			JsonRpcResponse(std::string jsonrpcVersion, std::string requestId,const JsonRpcError& error);
			JsonRpcResponse(const Json::Value& response);

			Json::Value toJson() const;
			std::string toJsonStr() const;

			std::string getId() const;

			Json::Value getResult() const;

			JsonRpcError::Ptr getError() const;

			bool isError() const;

			static JsonRpcResponse::Ptr generate(const std::string& jsonStr, bool* flag);
			static JsonRpcResponse::Ptr generate(const Json::Value& response, bool* flag);
			static JsonRpcResponse::Ptr generate(const Json::Value& request, const Json::Value result, bool* flag);
			static JsonRpcResponse::Ptr generate(const Json::Value& request, const JsonRpcError& error, bool* flag);
			static bool fieldsExist(const Json::Value& rpcresponseJson);
		private:
			Json::Value m_rpcResponse;
		};
	}
}