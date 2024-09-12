#include "rpcserverstub.h"

namespace Nano {
	namespace Rpc {
		RpcServerStub::RpcServerStub(short port)
		{
			m_rpcServer = RpcServer::Create(port);
			m_rpcServer->Init();
		}

		RpcServerStub::~RpcServerStub()
		{

		}

		void RpcServerStub::registReturn(std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap, RpcDoneCallback done)
		{
		}

		void RpcServerStub::registNotify(std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap)
		{
		}

		void RpcServerStub::run()
		{
			m_rpcServer->Start();
		}

		void RpcServerStub::stop()
		{
			m_rpcServer->Stop();
		}
	}
}