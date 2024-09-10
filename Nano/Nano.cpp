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
	BaseServer server(9800);
	server.Start();
	return 0;
}