#pragma once
#include <string>
#include <memory>
#include <map>
#include <list>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdarg.h>
#include <stdint.h>

namespace Nano {
	namespace Log {
		/// <summary>
		/// 日志级别
		/// </summary>
		enum class LogLevel {
			UNKOWN = 0,
			DEBUG = 1,
			INFO = 2,
			WARN = 3,
			ERROR = 4,
			FATAL = 5
		};

		/// <summary>
		/// 日志事件
		/// </summary>
		class LogEvent {
		private:
			const char* m_file = nullptr;	// 文件名
			int32_t m_line = 0;				// 行号
			uint32_t m_elapse = 0;			// 程序启动开始到现在的毫秒数
			uint32_t m_threadId = 0;		// 线程id
			uint32_t m_fiberId = 0;			// 协程id
			uint64_t m_time = 0;			// 时间戳
			std::string m_content;
		public:
			typedef std::shared_ptr<LogEvent> ptr;
			LogEvent();
		};

		/// <summary>
		/// 日志格式器
		/// </summary>
		class LogFormatter {
		private:

		public:
			typedef std::shared_ptr<LogFormatter> ptr;

			std::string format(LogEvent::ptr event);
		};

		/// <summary>
		/// 日志输出地
		/// </summary>
		class LogAppender {
		protected:
			LogLevel m_level;
		public:
			typedef std::shared_ptr<LogAppender> ptr;
			virtual ~LogAppender() {}
			virtual void log(LogLevel level, LogEvent::ptr event) = 0;
		};

		/// <summary>
		/// 日志器
		/// </summary>
		class Logger {
		private:
			std::string m_name;							// 日志名称
			std::list< LogAppender::ptr> m_appenders;	// Appender集合
			LogLevel m_level;							// 日志级别
		public:
			typedef std::shared_ptr<Logger> ptr;
			Logger(const std::string& name = "root");
			void log(LogLevel level, LogEvent::ptr event);

			void debug(LogEvent::ptr event);
			void info(LogEvent::ptr event);
			void warn(LogEvent::ptr event);
			void error(LogEvent::ptr event);
			void fatal(LogEvent::ptr event);

			void addAppender(LogAppender::ptr appender);
			void delAppender(LogAppender::ptr appender);
			LogLevel getLevel() const { return m_level; }
			void setLevel(LogLevel level) { m_level = level; }
		};

		/// <summary>
		/// 输出到控制台的Appender
		/// </summary>
		class StdoutLogAppender : public LogAppender {
		private:

		public:
			void log(LogLevel level, LogEvent::ptr event) override;

		};

		/// <summary>
		/// 输出到文件的Appender
		/// </summary>
		class FileLogAppender : public LogAppender {
		public:
			void log(LogLevel level, LogEvent::ptr event) override;
		};
	}
}