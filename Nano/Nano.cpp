#include "BaseServer.h"
#include "RpcService.h"
#include "RpcProcedure.h"

using namespace Nano::Log;
using namespace Nano::Utils;
using namespace Nano::JrpcProto;
using namespace Nano::Communication;


int main() {
	const char* jsonStr = "{\"jsonrpc\":\"2.0\",\"method\":\"subtract\",\"params\":{\"subtrahend\":23,\"minuend\":42},\"id\":\"132\"}";
	bool flag = false;
	JsonRpcRequest::Ptr request = JrpcRequestGenerator::generate(jsonStr, &flag);
	if (flag) {
		std::string jsonStr = request->toJsonStr();
		std::cout << jsonStr << std::endl;
	}
	else {
		std::cout << "Failed to generate json request" << std::endl;
	}

	return 0;
}