#include "utils.h"
#include "Log.h"

using namespace Nano::Log;
using namespace Nano::Utils;

int main() {
	auto asyncLogger = ASYNC_LOG_NAME("TEST");
	asyncLogger->addAppender(std::make_shared<StdoutLogAppender>());
	asyncLogger->setLevel(LogLevel::Level::DEBUG);
	ASYNC_LOG_DEBUG(asyncLogger, "main");

	
	system("pause");
    return 0;
}