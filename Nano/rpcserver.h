#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "RpcService.h"
#include "BaseServer.h"

namespace Nano {
	namespace Rpc {
		class RPCServer : public Nano::Communication::BaseServer
		{
		public:
			typedef std::shared_ptr<RPCServer> Ptr;

		};
	}
}