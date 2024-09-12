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
				initProcedure(std::forward<ParamNameAndTypes>(nameAndTypes)...);
			}

			explicit RpcProcedure(Func&& callback, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap) :
				m_callback(std::forward<Func>(callback)),
				m_params(std::move(paramsNameTypesMap))
			{
			}

			void invoke(Json::Value& request, const RpcDoneCallback& done) {
				validateRequest(request);
				m_callback(request, done);
			}

			void invoke(Json::Value& request) {
				validateRequest(request);
				m_callback(request);
			}
		private:
			void initProcedure() {}

			template<typename Name, typename Type, typename... Rest>
			void initProcedure(Name&& paramName, Type&& paramType, Rest&&... rest)
			{
				static_assert(std::is_same_v<std::decay_t<Type>, Json::ValueType>,
					"param type must be Json::ValueType");
				m_params.insert(std::make_pair(std::forward<Name>(paramName),
					std::forward<Type>(paramType)));
				initProcedure(std::forward<Rest>(rest)...);
			}

			void validateRequest(Json::Value& request) const;
			bool validateGeneric(Json::Value& request) const;
		private:
			Func m_callback;
			std::unordered_map<std::string, Json::ValueType> m_params;	// 参数列表
		};

		template <>
		inline void RpcProcedure<ProcedureReturnCallback>::validateRequest(Json::Value& request) const {
			if (!validateGeneric(request))
			{
				int code = JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams);
				std::string detail = JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams);
				throw RpcException(code, detail);
			}
		}

		template <>
		inline void RpcProcedure<ProcedureNotifyCallback>::validateRequest(Json::Value& request) const
		{
			if (!validateGeneric(request))
			{
				int code = JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams);
				std::string detail = JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams);
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
				std::string key = it.key().asString();  // 获取键
				Json::Value value = *it;  // 获取对应的值
				if (m_params.find(key) == m_params.end()) {
					return false;
				}
				auto standradItem = m_params.find(key);
				if (standradItem->second != value.type()) {
					return false;
				}
			}
			return true;
		}
		typedef RpcProcedure<ProcedureReturnCallback> ProcedureReturn;
		typedef RpcProcedure<ProcedureNotifyCallback> ProcedureNotify;
	}
}