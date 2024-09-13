#include "rpcclient.h"

namespace Nano {
	namespace Rpc {
		RpcClient::RpcClient()
		{
		}

		RpcClient::~RpcClient()
		{
			this->m_callRecords.clear();
		}

		void RpcClient::Init()
		{
			this->m_ceventHandler->AddDataReadyHandler(weak_from_this());
		}

		bool RpcClient::callReturnProcedure(JrpcProto::JsonRpcRequest::Ptr request, const RpcDoneCallback callback)
		{
			// called
			if (m_callRecords.find(request->getId()) != m_callRecords.end())
				return false;
			// not called
			// 1. record
			RpcCallRecord::Ptr reRecord = std::make_shared<RpcCallRecord>(request);
			m_callRecords.emplace(request->getId(), std::make_pair(reRecord, callback));
			// 2. call
			std::string jsonStyleStr = request->toJsonStr();
			char* buffer = nullptr;
			int len = 0;
			if (TransferCode::Code::encode(jsonStyleStr, &buffer, &len))
			{
				this->Send(buffer, len);
				delete[] buffer;
				buffer = nullptr;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool RpcClient::callNotifyProcedure(JrpcProto::JsonRpcRequest::Ptr request)
		{
			std::string jsonStyleStr = request->toJsonStr();
			char* buffer = nullptr;
			int len = 0;
			if (TransferCode::Code::encode(jsonStyleStr, &buffer, &len))
			{
				this->Send(buffer, len);
				delete[] buffer;
				buffer = nullptr;
				return true;
			}
			else
			{
				return false;
			}
		}

		RpcClient::RpcCallRecord::Ptr RpcClient::getReturnCallRecord(const std::string& id)
		{
			auto it = m_callRecords.find(id);
			if (it != m_callRecords.end())
				return it->second.first;
			return nullptr;
		}

		void RpcClient::OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
			ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "RpcClient") << packet->ToString() << std::endl;
			std::string responseJsonStr = TransferCode::Code::decode(packet->m_data, packet->m_size);
			bool generateResult = false;
			Nano::JrpcProto::JsonRpcResponse::Ptr response = Nano::JrpcProto::JsonRpcResponse::generate(responseJsonStr, &generateResult);

			if (generateResult)
			{
				auto it = m_callRecords.find(response->getId());
				if (it != m_callRecords.end())
				{
					it->second.first->response = response;
					it->second.second(response->getResult());
				}
			}
		}
	}
}