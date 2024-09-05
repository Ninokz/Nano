#include "Log.h"

namespace Nano {
	namespace Log {
		Logger::Logger(const std::string& name)
		{

		}
		void Logger::log(LogLevel level, LogEvent::ptr event)
		{
		}

		void Logger::debug(LogEvent::ptr event)
		{
			log(LogLevel::DEBUG, event);
		}

		void Logger::info(LogEvent::ptr event)
		{
			log(LogLevel::INFO, event);
		}

		void Logger::warn(LogEvent::ptr event)
		{
			log(LogLevel::WARN, event);
		}

		void Logger::error(LogEvent::ptr event)
		{
			log(LogLevel::ERROR, event);
		}

		void Logger::fatal(LogEvent::ptr event)
		{
			log(LogLevel::FATAL, event);
		}

		void Logger::addAppender(LogAppender::ptr appender)
		{
			m_appenders.push_back(appender);
		}

		void Logger::delAppender(LogAppender::ptr appender)
		{
			for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it)
			{
				if (*it == appender)
				{
					m_appenders.erase(it);
					break;
				}
			}
		}
	}
}