#include "rpcclient.h"

namespace Nano {
	namespace Rpc {
		RpcClient::RpcClient()
		{
		}

		RpcClient::~RpcClient()
		{
		}

		bool RpcClient::connect(const std::string& ip, short port)
		{
			try {
				this->m_endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip), port);
				boost::system::error_code error = boost::asio::error::host_not_found;
				m_session->getSocket().connect(m_endpoint, error);
				if (error)
				{
					std::cerr << "Connect failed: " << error.message() << std::endl;
					return false;
				}
				else
				{
					m_session->Start();
					m_clientThread = std::thread([this]() {
						m_running = true;
						m_ioc.run();
					});
					return true;
				}
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				return false;
			}
		}

		void RpcClient::disconnect()
		{
			m_running = false;
			m_session->Close();
			m_ioc.stop();
			if (m_clientThread.joinable())
				m_clientThread.join();
		}

		bool RpcClient::callReturnProcedure(JrpcProto::JsonRpcRequest::Ptr request)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_callRecords.find(request->m_id) == m_callRecords.end())
			{
				// not called
				// 1. record
				RpcCallRecord::Ptr reRecord = std::make_shared<RpcCallRecord>(request);
				m_callRecords[request->m_id] = reRecord;
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
					std::cerr << "callReturnProcedure: encode failed" << std::endl;
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
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_callRecords.find(request->m_id) == m_callRecords.end())
			{
				// not called
				// 1. record
				RpcCallRecord::Ptr reRecord = std::make_shared<RpcCallRecord>(request);
				m_callRecords[request->m_id] = reRecord;
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
					std::cerr << "callNotifyProcedure: encode failed" << std::endl;
					return false;
				}
			}
			else
			{
				// already called
				return false;
			}
		}

		RpcCallRecord::Ptr RpcClient::getCallRecord(std::string id)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_callRecords.find(id) != m_callRecords.end())
			{
				return m_callRecords[id];
			}
			else
			{
				return nullptr;
			}
		}

		void RpcClient::removeCallRecord(std::string id)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_callRecords.find(id) != m_callRecords.end())
			{
				m_callRecords.erase(id);
			}
		}

		void RpcClient::OnDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
			if (sender && packet)
			{
				std::string response = TransferCode::Code::decode(packet->m_data, packet->m_size);
				bool parseResult = false;
				JrpcProto::JsonRpcResponse::Ptr jsresponse = JrpcProto::JrpcResponseParser::parse(response, &parseResult);
				std::lock_guard<std::mutex> lock(m_mutex);
				if (parseResult)
				{			
					std::cout << packet->ToString() << std::endl;
					if (this->m_callRecords.find(jsresponse->m_id) != this->m_callRecords.end())
					{
						this->m_callRecords[jsresponse->m_id]->response = jsresponse;
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
