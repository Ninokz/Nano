#include "jrpcproto.h"

namespace Nano {
	namespace JrpcProto {
		Json::Value JsonRpcRequest::toJson() const
		{
			Json::Value request;
			request["jsonrpc"] = m_ver;
			request["method"] = m_method;
			request["params"] = m_params;
			request["id"] = m_id;
			return request;
		}

		std::string JsonRpcRequest::toJsonStr() const
		{
			Json::Value request = toJson();
			Json::StreamWriterBuilder writer;
			return Json::writeString(writer, request);
		}

		JsonRpcRequest::Ptr JrpcRequestGenerator::generate(const std::string& jsonStr, bool* flag)
		{
			try {
				JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>();
				Json::CharReaderBuilder readerBuilder;
				Json::Value root;
				std::string errs;

				std::istringstream iss(jsonStr);
				if (!Json::parseFromStream(readerBuilder, iss, &root, &errs)) {
					std::cerr << "Failed to parse JSON: " << errs << std::endl;
					*flag = false;
					return nullptr;
				}

				if (!root.isMember("jsonrpc") || root["jsonrpc"].asString() != "2.0") {
					std::cerr << "Invalid or missing 'jsonrpc' field." << std::endl;
					*flag = false;
					return nullptr;
				}

				if (!root.isMember("method") || !root["method"].isString()) {
					std::cerr << "Invalid or missing 'method' field." << std::endl;
					*flag = false;
					return nullptr;
				}

				if (!root.isMember("id") || !root["id"].isString()) {
					/// JSON-RPC 2.0: A Notification is a Request object without an "id" member
					request->m_id = "";
				}
				else
				{
					/// JSON-RPC 2.0: A Request object that is a method call is identified by the presence of an "id" member
					request->m_id = root["id"].asString();
				}

				request->m_ver = root["jsonrpc"].asString();
				request->m_method = root["method"].asString();
				request->m_params = root["params"];
				*flag = true;
				return request;
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				*flag = false;
				return nullptr;
			}
		}

		JsonRpcRequest::Ptr JrpcRequestGenerator::generate(std::string method, std::string id, Json::Value params)
		{
			JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>();
			request->m_ver = "2.0";
			request->m_method = method;
			request->m_params = params;
			request->m_id = id;
			return request;
		}

		JsonRpcRequest::Ptr JrpcRequestGenerator::generate(std::string method, Json::Value params)
		{
			JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>();
			request->m_ver = "2.0";
			request->m_method = method;
			request->m_params = params;
			request->m_id = "";
			return request;
		}

		Json::Value JsonRpcError::toJson() const
		{
			Json::Value error;
			error["code"] = static_cast<int>(m_code);
			error["message"] = m_message;
			return error;
		}

		std::string JsonRpcError::toJsonStr() const
		{
			Json::Value error = toJson();
			Json::StreamWriterBuilder writer;
			return Json::writeString(writer, error);
		}

		std::string JsonRpcError::getErrorMessage(JsonRpcErrorCode code)
		{
			switch (code) {
#define XX(name) case JsonRpcErrorCode::name: return #name;
				XX(ParseError)
					XX(InvalidRequest)
					XX(MethodNotFound)
					XX(InvalidParams)
					XX(InternalError)
#undef XX
			default:
				return "Unknown error";
			}
		}

		JsonRpcError::JsonRpcErrorCode JsonRpcError::toErrorCode(const int code)
		{
			switch (code) {
#define XX(name) case static_cast<int>(JsonRpcErrorCode::name): return JsonRpcErrorCode::name;
				XX(ParseError)
					XX(InvalidRequest)
					XX(MethodNotFound)
					XX(InvalidParams)
					XX(InternalError)
#undef XX
			default:
				return JsonRpcErrorCode::InternalError;
			}
		}

		int JsonRpcError::toInt(JsonRpcErrorCode code)
		{
			return static_cast<int>(code);
		}

		Json::Value JsonRpcResponse::toJson() const
		{
			Json::Value response;
			response["jsonrpc"] = jsonrpc;
			response["id"] = m_id;
			if (error) {
				response["error"] = error->toJson();
			}
			else {
				response["result"] = result;
			}
			return response;
		}

		std::string JsonRpcResponse::toJsonStr() const
		{
			Json::Value error = toJson();
			Json::StreamWriterBuilder writer;
			return Json::writeString(writer, error);
		}

		JsonRpcResponse::Ptr JrpcResponseParser::parse(const std::string& jsonStr, bool* flag)
		{
			try {
				/// 先确定是错误还是正常返回， 通过解析是否存在error字段判断
				Json::CharReaderBuilder readerBuilder;
				Json::Value root;
				std::string errs;
				std::istringstream iss(jsonStr);
				if (!Json::parseFromStream(readerBuilder, iss, &root, &errs)) {
					*flag = false;
					return nullptr;
				}
				if (JrpcResponseParser::fieldsExist(root) == false) {
					*flag = false;
					return nullptr;
				}
				std::string jsonrpc = root["jsonrpc"].asString();
				if (jsonrpc != "2.0")
				{
					*flag = false;
					return nullptr;
				}
				std::string id = root["id"].asString();
				if (root.isMember("error"))
				{
					/// 错误返回解析 JsonRpcError::Ptr error;
					*flag = true;
					Json::Value error = root["error"];
					JsonRpcError::JsonRpcErrorCode code = JsonRpcError::toErrorCode(error["code"].asInt());
					JsonRpcResponse::Ptr response = std::make_shared<JsonRpcResponse>(jsonrpc, id, code);
					return response;
				}
				else if (root.isMember("result"))
				{
					/// 正常返回解析 Json::Value result;
					*flag = true;
					Json::Value result = root["result"];
					JsonRpcResponse::Ptr response = std::make_shared<JsonRpcResponse>(jsonrpc, id, result);
					return response;
				}
				else
				{
					*flag = false;
					return nullptr;
				}
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				*flag = false;
				return nullptr;
			}
		}

		JsonRpcResponse::Ptr JrpcResponseParser::parse(const Json::Value& root, bool* flag)
		{
			try {
				if (JrpcResponseParser::fieldsExist(root) == false)
				{
					*flag = false;
					return nullptr;
				}
				std::string jsonrpc = root["jsonrpc"].asString();
				if (jsonrpc != "2.0")
				{
					*flag = false;
					return nullptr;
				}
				std::string id = root["id"].asString();
				if (root.isMember("error"))
				{
					/// 错误返回解析 JsonRpcError::Ptr error;
					*flag = true;
					Json::Value error = root["error"];
					JsonRpcError::JsonRpcErrorCode code = JsonRpcError::toErrorCode(error["code"].asInt());
					JsonRpcResponse::Ptr response = std::make_shared<JsonRpcResponse>(jsonrpc, id, code);
					return response;
				}
				else if (root.isMember("result"))
				{
					/// 正常返回解析 Json::Value result;
					*flag = true;
					Json::Value result = root["result"];
					JsonRpcResponse::Ptr response = std::make_shared<JsonRpcResponse>(jsonrpc, id, result);
					return response;
				}
				else
				{
					*flag = false;
					return nullptr;
				}
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				*flag = false;
				return nullptr;
			}
		}

		bool JrpcResponseParser::fieldsExist(const Json::Value& root)
		{
			if (root.isMember("error") && root.isMember("result"))
			{
				return false;
			}
			if (!root.isMember("error") && !root.isMember("result"))
			{
				return false;
			}
			if (!root.isMember("jsonrpc") || !root.isMember("id"))
			{
				return false;
			}
			return true;
		}
	}
}