#pragma once
#include "Log.h"

using namespace Nano::Log;
namespace Nano {
	namespace Log {
		const char* consoleDetailParrten = "%d{%Y-%m-%d %H:%M:%S}%T[%p]%T[%c]%T%t%T%N%T%f:%l%T%m%n";
		const char* consoleStdParrten = "[%p]%T%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%m%n";
		static void InitLoggers()
		{
			auto stdlogger = ASYNC_LOG_NAME("STD_LOGGER");
			LogFormatter::ptr stdFormat = std::make_shared<LogFormatter>(consoleStdParrten);
			ANSIColorStdoutLogAppender::ptr stdAppender = std::make_shared<ANSIColorStdoutLogAppender>();
			stdAppender->setFormatter(stdFormat);
			stdlogger->addAppender(stdAppender);
			stdlogger->setLevel(LogLevel::Level::DEBUG);
		}
	}
}