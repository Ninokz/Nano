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

		bool RpcClient::callReturnProcedure(JrpcProto::JsonRpcRequest::Ptr request, const RpcResponseCallback callback)
		{
			if (m_callRecords.find(request->m_id) == m_callRecords.end())
			{
				// not called
				// 1. record
				RpcCallRecord::Ptr reRecord = std::make_shared<RpcCallRecord>(request);
				m_callRecords.emplace(request->m_id, std::make_pair(reRecord, callback));
				// 2. call
				std::string jsonStyleStr = request->toJsonStr();
				char* buffer = nullptr;
				int len = 0;
				if (TransferCode::Code::encode(jsonStyleStr, &buffer, &len))
				{
					m_session->Send(buffer, len);
					delete buffer;
					buffer = nullptr;
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				// already called
				return false;
			}
		}

		bool RpcClient::callNotifyProcedure(JrpcProto::JsonRpcRequest::Ptr request)
		{
			if (m_callRecords.find(request->m_id) == m_callRecords.end())
			{
				std::string jsonStyleStr = request->toJsonStr();
				char* buffer = nullptr;
				int len = 0;
				if (TransferCode::Code::encode(jsonStyleStr, &buffer, &len))
				{
					m_session->Send(buffer, len);
					delete buffer;
					buffer = nullptr;
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				// already called
				return false;
			}
		}

		RpcCallRecord::Ptr RpcClient::getReturnCallRecord(const std::string& id)
		{
			auto it = m_callRecords.find(id);
			if (it != m_callRecords.end())
				return it->second.first;
			return nullptr;
		}

		void RpcClient::OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
			if (sender && packet)
			{
				std::string response = TransferCode::Code::decode(packet->m_data, packet->m_size);
				bool parseResult = false;
				JrpcProto::JsonRpcResponse::Ptr jsresponse = JrpcProto::JrpcResponseParser::parse(response, &parseResult);
				if (parseResult)
				{
					std::cout << packet->ToString() << std::endl;
					if (this->m_callRecords.find(jsresponse->m_id) != this->m_callRecords.end())
					{
						m_callRecords[jsresponse->m_id].first->response = jsresponse;
						m_callRecords[jsresponse->m_id].second(jsresponse->result);
					}
					else
					{
						std::cerr << packet->ToString() << std::endl;
					}
				}
				else
				{
					std::cerr << packet->ToString() << std::endl;
				}
			}
		}
	}
}