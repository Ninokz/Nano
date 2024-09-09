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

			JsonRpcRequest(const std::string& jsonrpcVersion, const std::string& methodName, const Json::Value& parameters, std::string requestId)
				: m_ver(jsonrpcVersion), m_method(methodName), m_params(parameters), m_id(requestId) {}
			static bool fromJson(const std::string& jsonStr, JsonRpcRequest& request);
		public:
			std::string m_ver;
			std::string m_method;
			Json::Value m_params;
			std::string m_id;
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
			std::string m_id;

            // 构造函数：成功响应
            JsonRpcResponse(const std::string& jsonrpcVersion, std::string requestId, const Json::Value& resultValue)
                : jsonrpc(jsonrpcVersion), m_id(requestId), result(resultValue), error(nullptr) {}

            // 构造函数：错误响应
            JsonRpcResponse(const std::string& jsonrpcVersion, std::string requestId, JsonRpcError::JsonRpcErrorCode errorCode)
                : jsonrpc(jsonrpcVersion), m_id(requestId), error(new JsonRpcError(errorCode)) {}

			~JsonRpcResponse();

			std::string toJson() const;
        };

        class JrpcRequestGenerator {
        public:
            // This function accepts a variable number of key-value pairs.
            template <typename... Args>
            static std::string generate(const std::string& method, const int id, const Args&... args) {
                // Create the root object for the JSON-RPC request
                Json::Value root;
                root["jsonrpc"] = "2.0";   // JSON-RPC version
                root["method"] = method;   // Method name
                root["id"] = id;           // Request ID

                // Pack the variadic arguments into a JSON object (key-value pairs)
                Json::Value params(Json::objectValue);
                addParams(params, args...);  // Recursively add the variadic key-value pairs to the JSON object

                root["params"] = params;    // Add the parameters to the root object

                // Convert the JSON object to a string and return it
                Json::StreamWriterBuilder writer;
                return Json::writeString(writer, root);
            }

        private:
            // Base case for recursion: no arguments
            static void addParams(Json::Value& params) {
            }

            // Recursive function to add variadic key-value arguments to the JSON object
            template <typename Key, typename Value, typename... Args>
            static void addParams(Json::Value& params, const Key& key, const Value& value, const Args&... args) {
                params[key] = value;       // Add the current key-value pair to the params object
                addParams(params, args...);  // Recur for the remaining arguments
            }
        };

		class JrpcResponseParser {
		public:

		};
	}
}