#include "jrpcproto.h"


namespace Nano {
	namespace JrpcProto {
		bool JsonRpcRequest::fromJson(const std::string& jsonStr, JsonRpcRequest& request)
        {
            Json::CharReaderBuilder readerBuilder;
            Json::Value root;
            std::string errs;

            std::istringstream iss(jsonStr);
            if (!Json::parseFromStream(readerBuilder, iss, &root, &errs)) {
                std::cerr << "Failed to parse JSON: " << errs << std::endl;
                return false;
            }

            if (!root.isMember("jsonrpc") || root["jsonrpc"].asString() != "2.0") {
                std::cerr << "Invalid or missing 'jsonrpc' field." << std::endl;
                return false;
            }

            if (!root.isMember("method") || !root["method"].isString()) {
                std::cerr << "Invalid or missing 'method' field." << std::endl;
                return false;
            }

            if (!root.isMember("id") || !root["id"].isInt()) {
                std::cerr << "Invalid or missing 'id' field." << std::endl;
                return false;
            }
            request.m_ver = root["jsonrpc"].asString();
            request.m_method = root["method"].asString();
            request.m_params = root["params"];
            request.m_id = root["id"].asInt();
            return true;
        }

        Json::Value JsonRpcError::toJson() const
        {
            Json::Value error;
            error["code"] = static_cast<int>(code);
            error["message"] = message;
            return error;
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

        JsonRpcResponse::~JsonRpcResponse()
        {
            if (error) {
                delete error;
                error = nullptr;
            }
        }

        std::string JsonRpcResponse::toJson() const
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

            Json::StreamWriterBuilder writer;
            return Json::writeString(writer, response);
        }
    }
}