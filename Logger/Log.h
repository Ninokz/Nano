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
		/// ��־����
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
		/// ��־�¼�
		/// </summary>
		class LogEvent {
		private:
			const char* m_file = nullptr;	// �ļ���
			int32_t m_line = 0;				// �к�
			uint32_t m_elapse = 0;			// ����������ʼ�����ڵĺ�����
			uint32_t m_threadId = 0;		// �߳�id
			uint32_t m_fiberId = 0;			// Э��id
			uint64_t m_time = 0;			// ʱ���
			std::string m_content;
		public:
			typedef std::shared_ptr<LogEvent> ptr;
			LogEvent();
		};

		/// <summary>
		/// ��־��ʽ��
		/// </summary>
		class LogFormatter {
		private:

		public:
			typedef std::shared_ptr<LogFormatter> ptr;

			std::string format(LogEvent::ptr event);
		};

		/// <summary>
		/// ��־�����
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
		/// ��־��
		/// </summary>
		class Logger {
		private:
			std::string m_name;							// ��־����
			std::list< LogAppender::ptr> m_appenders;	// Appender����
			LogLevel m_level;							// ��־����
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
		/// ���������̨��Appender
		/// </summary>
		class StdoutLogAppender : public LogAppender {
		private:

		public:
			void log(LogLevel level, LogEvent::ptr event) override;

		};

		/// <summary>
		/// ������ļ���Appender
		/// </summary>
		class FileLogAppender : public LogAppender {
		public:
			void log(LogLevel level, LogEvent::ptr event) override;
		};
	}
}