#pragma once
#include "Log.h"

using namespace Nano::Log;
namespace Nano {
	namespace Log {
		const char* consoleDetailParrten = "%d{%Y-%m-%d %H:%M:%S}%T[%p]%T[%c]%T%t%T%N%T%f:%l%T%m%n";
		const char* consoleStdParrten = "%d{%Y-%m-%d %H:%M:%S}%T[%p]%T%t%T%N%T%m%n";
		static void InitLoggers()
		{
			auto std_serverlogger = ASYNC_LOG_NAME("SERVER_STD_LOGGER");
			LogFormatter::ptr stdFormat = std::make_shared<LogFormatter>(consoleStdParrten);
			ANSIColorStdoutLogAppender::ptr stdAppender = std::make_shared<ANSIColorStdoutLogAppender>();
			stdAppender->setFormatter(stdFormat);
			std_serverlogger->addAppender(stdAppender);
			std_serverlogger->setLevel(LogLevel::Level::DEBUG);
		}
	}
}