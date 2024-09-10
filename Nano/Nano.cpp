#include "utils.h"
#include "Log.h"
#include "Loginit.h"
#include "Env.h"
#include "jrpcproto.h"
#include "BaseServer.h"

using namespace Nano::Log;
using namespace Nano::Utils;
using namespace Nano::JrpcProto;
using namespace Nano::Communication;

int main() {
	InitLoggers();
	auto logger = ASYNC_LOG_NAME("SERVER_STD_LOGGER");

	ASYNC_LOG_FATAL(logger, "main");
	ASYNC_LOG_DEBUG(logger, "main");
	ASYNC_LOG_INFO(logger, "main");
	ASYNC_LOG_WARN(logger, "main");
	ASYNC_LOG_ERROR(logger, "main");

	system("pause");
	return 0;
}