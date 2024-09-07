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
		class LogLevel {
		public:
			enum class Level {
				FATAL = 0,
				ALERT = 100,
				ERROR = 200,
				WARN = 300,
				NOTICE = 400,
				INFO = 500,
				DEBUG = 600
			};

			const static char* ToString(LogLevel::Level level);

			static LogLevel::Level FromString(const std::string& str);
		};

		class LogEvent {
		public:
			typedef std::shared_ptr<LogEvent> ptr;

			LogEvent(const std::string& logger_name, LogLevel::Level level, const char* file, int32_t line
				, int64_t elapse, uint32_t thread_id, uint64_t fiber_id, time_t time, const std::string& thread_name);

			LogLevel::Level getLevel() const { return m_level; }
			std::string getContent() const { return m_ss.str(); }
			std::string getFile() const { return m_file; }
			int32_t getLine() const { return m_line; }
			int64_t getElapse() const { return m_elapse; }
			uint32_t getThreadId() const { return m_threadId; }
			uint64_t getFiberId() const { return m_fiberId; }
			time_t getTime() const { return m_time; }
			const std::string& getThreadName() const { return m_threadName; }
			std::stringstream& getSS() { return m_ss; }
			const std::string& getLoggerName() const { return m_loggerName; }

			void printf(const char* fmt, ...);
			void vprintf(const char* fmt, va_list ap);
		private:
			/// ��־����
			LogLevel::Level m_level;
			/// ��־���ݣ�ʹ��stringstream�洢��������ʽд����־
			std::stringstream m_ss;
			/// �ļ���
			const char* m_file = nullptr;
			/// �к�
			int32_t m_line = 0;
			/// ����־��������ʼ����ǰ�ĺ�ʱ
			int64_t m_elapse = 0;
			/// �߳�id
			uint32_t m_threadId = 0;
			/// Э��id
			uint64_t m_fiberId = 0;
			/// UTCʱ���
			time_t m_time;
			/// �߳�����
			std::string m_threadName;
			/// ��־������
			std::string m_loggerName;
		};

		class LogFormatter {

		};

		class LogAppender {

		};

		class Logger {

		};

		class StdoutLogAppender : public LogAppender {

		};

		class FileLogAppender : public LogAppender {

		};
	}
}