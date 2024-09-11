#pragma once
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <type_traits>

#include <json/json.h>

#include "rpcexception.h"
#include "jrpcproto.h"
#include "nocopyable.h"

namespace Nano {
	namespace Rpc {
		typedef std::function<void(Json::Value response)> RpcDoneCallback;
		typedef std::function<void(Json::Value&, const RpcDoneCallback&)> ProcedureReturnCallback;
		typedef std::function<void(Json::Value&)> ProcedureNotifyCallback;

		template <typename Func>
		class RpcProcedure : public Noncopyable
		{
		public:
			typedef std::shared_ptr<RpcProcedure> Ptr;
		public:
			template<typename... ParamNameAndTypes>
			explicit RpcProcedure(Func&& callback, ParamNameAndTypes&&... nameAndTypes) :
				m_callback(std::forward<Func>(callback))
			{
				constexpr int n = sizeof...(nameAndTypes);
				static_assert(n % 2 == 0, "procedure must have param name and type pairs");
				if constexpr (n > 0)
					initProcedure(nameAndTypes...);
			}

			void invoke(Json::Value& request, const RpcDoneCallback& done);
			void invoke(Json::Value& request);
		private:
			template<typename Name, typename Type, typename... ParamNameAndTypes>
			void initProcedure(Name paramName, Type parmType, ParamNameAndTypes &&... nameAndTypes)
			{
				static_assert(std::is_same_v<Type, Json::ValueType>, "param type must be json::ValueType");
			}

			void validateRequest(Json::Value& request) const;
			bool validateGeneric(Json::Value& request) const;
		private:
			Func m_callback;
			std::unordered_map<std::string, Json::ValueType> m_params;
		};

		template <>
		void RpcProcedure<ProcedureReturnCallback>::validateRequest(Json::Value& request) const {
			if (!validateGeneric(request))
			{
				int code = JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams);
				const char* detail = JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams).c_str();
				throw RpcException(code, detail);
			}
		}

		template <>
		void RpcProcedure<ProcedureNotifyCallback>::validateRequest(Json::Value& request) const
		{
			if (!validateGeneric(request))
			{
				int code = JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams);
				const char* detail = JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams).c_str();
				throw RpcException(code, detail);
			}
		}

		template <typename Func>
		bool RpcProcedure<Func>::validateGeneric(Json::Value& request) const {
			auto params = request.isMember("params") ? request["params"] : Json::Value(Json::nullValue);
			if (params == Json::nullValue || !params.isObject()) {
				return false;
			}
			for (Json::Value::const_iterator it = params.begin(); it != params.end(); ++it) {
				std::string key = it.key().asString();  // ��ȡ��
				Json::Value value = *it;  // ��ȡ��Ӧ��ֵ		
				if (m_params.find(key) == m_params.end()) {
					return false;
				}
				if (m_params[key] != value.type()) {
					return false;
				}
			}
			return true;
		}

		template <>
		void RpcProcedure<ProcedureReturnCallback>::invoke(Json::Value& request, const RpcDoneCallback& done) {
			validateRequest(request);
			m_callback(request, done);
		}

		template <>
		void RpcProcedure<ProcedureNotifyCallback>::invoke(Json::Value& request) {
			validateRequest(request);
			m_callback(request);
		}
	}
}