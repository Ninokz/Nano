#pragma once
#include <functional>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_map>

#include "nocopyable.h"
#include "rpcprocedure.h"
#include "jrpcproto.h"


namespace Nano {
	namespace Rpc {
		class RpcService : public Noncopyable {
		public:
			typedef std::unique_ptr<ProcedureReturn> ProcedureReturnPtr;
			typedef std::unique_ptr<ProcedureNotify> ProcedureNotifyPtr;
			typedef std::unordered_map<std::string, ProcedureReturnPtr> ProcedureReturnMap;
			typedef std::unordered_map<std::string, ProcedureNotifyPtr> ProcedureNotifyMap;
		public:
			typedef std::unique_ptr<RpcService> Ptr;

			void addProcedureReturn(std::string methodName, ProcedureReturnPtr p)
			{
				assert(m_procedureReturn.find(methodName) == m_procedureReturn.end());
				m_procedureReturn.emplace(std::move(methodName), std::move(p));
			}

			void addProcedureNotify(std::string methodName, ProcedureNotifyPtr p)
			{
				assert(m_procedureNotfiy.find(methodName) == m_procedureNotfiy.end());
				m_procedureNotfiy.emplace(std::string(methodName), std::move(p));
			}

			void callProcedureReturn(std::string methodName,Json::Value& request,const RpcDoneCallback& done);

			void callProcedureNotify(std::string methodName, Json::Value& request);

		private:
			ProcedureReturnMap m_procedureReturn;
			ProcedureNotifyMap m_procedureNotfiy;
		};
	}
}