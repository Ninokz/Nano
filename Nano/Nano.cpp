#include "utils.h"
#include "Log.h"

using namespace Nano::Log;
using namespace Nano::Utils;

int main() {
    auto logger = Nano::LOG_NAME("example");
	logger->addAppender(std::make_shared<ANSIColorStdoutLogAppender>());
	logger->setLevel(LogLevel::Level::DEBUG);

    // Log a message at different levels

	LOG_FATAL(logger, "main");
	LOG_DEBUG(logger, "main");
	LOG_INFO(logger,  "main");
	LOG_WARN(logger, "main");
	LOG_ERROR(logger,  "main");
    return 0;
}