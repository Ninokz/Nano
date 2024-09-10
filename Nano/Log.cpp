#include "Log.h"

namespace Nano {
	namespace Log {
#pragma region LogLevel
		const char* LogLevel::ToString(LogLevel::Level level)
		{
			switch (level) {
#define XX(name) case LogLevel::Level::name: return #name;
				XX(FATAL);
				XX(ERRO);
				XX(WARN);
				XX(INFO);
				XX(DEBUG);
#undef XX
			default:
				return "UNKNOWN";
			}
			return "UNKNOWN";
		}

		LogLevel::Level LogLevel::FromString(const std::string& str)
		{
#define XX(level, v) \
				if (str == #v) { \
					return LogLevel::Level::level; \
				}
			XX(FATAL, fatal);
			XX(ERRO, erro);
			XX(WARN, warn);
			XX(INFO, info);
			XX(DEBUG, debug);

			XX(FATAL, FATAL);
			XX(ERRO, ERRO);
			XX(WARN, WARN);
			XX(INFO, INFO);
			XX(DEBUG, DEBUG);
#undef XX
			return LogLevel::Level::UNKNOWN;
		}
#pragma endregion

#pragma region LogEvent
		LogEvent::LogEvent(const std::string& logger_name, LogLevel::Level level,
			const char* file, int32_t line, uint64_t thread_id,
			time_t time, const std::string& sender)
			: m_level(level)
			, m_file(file)
			, m_line(line)
			, m_threadId(thread_id)
			, m_time(time)
			, m_sender(sender)
			, m_loggerName(logger_name) {
		}

		void LogEvent::printf(const char* fmt, ...) {
			va_list ap;
			va_start(ap, fmt);
			vprintf(fmt, ap);
			va_end(ap);
		}

		void LogEvent::vprintf(const char* fmt, va_list ap) {
			va_list ap_copy;
			va_copy(ap_copy, ap);
			int needed = vsnprintf(nullptr, 0, fmt, ap_copy);
			va_end(ap_copy);
			if (needed <= 0) {
				return;
			}
			std::unique_ptr<char[]> buf(new char[needed + 1]);
			vsnprintf(buf.get(), needed + 1, fmt, ap);
			m_ss << std::string(buf.get(), needed);
		}
#pragma endregion

#pragma region Formatter

#pragma region Item
		class MessageFormatItem : public LogFormatter::FormatItem {
		public:
			MessageFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << event->getContent();
			}
		};

		class LevelFormatItem : public LogFormatter::FormatItem {
		public:
			LevelFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << LogLevel::ToString(event->getLevel());
			}
		};

		class LoggerNameFormatItem : public LogFormatter::FormatItem {
		public:
			LoggerNameFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << event->getLoggerName();
			}
		};

		class ThreadIdFormatItem : public LogFormatter::FormatItem {
		public:
			ThreadIdFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << event->getThreadId();
			}
		};

		class SenderFormatItem : public LogFormatter::FormatItem {
		public:
			SenderFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << event->getSender();
			}
		};

		class DateTimeFormatItem : public LogFormatter::FormatItem {
		public:
			DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
				: m_format(format) {
				if (m_format.empty()) {
					m_format = "%Y-%m-%d %H:%M:%S";
				}
			}

			void format(std::ostream& os, LogEvent::ptr event) override {
				struct tm tm;
				time_t time = event->getTime();
				if (localtime_s(&tm, &time) == 0) {
					char buf[64];
					strftime(buf, sizeof(buf), m_format.c_str(), &tm);
					os << buf;
				}
				else {
					os << "[Invalid Time]";
				}
			}
		private:
			std::string m_format;
		};

		class FileNameFormatItem : public LogFormatter::FormatItem {
		public:
			FileNameFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << event->getFile();
			}
		};

		class LineFormatItem : public LogFormatter::FormatItem {
		public:
			LineFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << event->getLine();
			}
		};

		class NewLineFormatItem : public LogFormatter::FormatItem {
		public:
			NewLineFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << std::endl;
			}
		};

		class StringFormatItem : public LogFormatter::FormatItem {
		public:
			StringFormatItem(const std::string& str)
				:m_string(str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << m_string;
			}
		private:
			std::string m_string;
		};

		class TabFormatItem : public LogFormatter::FormatItem {
		public:
			TabFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << "\t";
			}
		};

		class PercentSignFormatItem : public LogFormatter::FormatItem {
		public:
			PercentSignFormatItem(const std::string& str) {}
			void format(std::ostream& os, LogEvent::ptr event) override {
				os << "%";
			}
		};
#pragma endregion

		LogFormatter::LogFormatter(const std::string& pattern)
			: m_pattern(pattern) {
			init();
		}

		void LogFormatter::init() {
			// 按顺序存储解析到的pattern项
			// 每个pattern包括一个整数类型和一个字符串，类型为0表示该pattern是常规字符串，为1表示该pattern需要转义
			// 日期格式单独用下面的dataformat存储
			std::vector<std::pair<int, std::string>> patterns;
			// 临时存储常规字符串
			std::string tmp;
			// 日期格式字符串，默认把位于%d后面的大括号对里的全部字符都当作格式字符，不校验格式是否合法
			std::string dateformat;
			// 是否解析出错
			bool error = false;

			// 是否正在解析常规字符，初始时为true
			bool parsing_string = true;
			// 是否正在解析模板字符，%后面的是模板字符
			// bool parsing_pattern = false;

			size_t i = 0;
			while (i < m_pattern.size()) {
				std::string c = std::string(1, m_pattern[i]);
				if (c == "%") {
					if (parsing_string) {
						if (!tmp.empty()) {
							patterns.push_back(std::make_pair(0, tmp));
						}
						tmp.clear();
						parsing_string = false; // 在解析常规字符时遇到%，表示开始解析模板字符
						// parsing_pattern = true;
						i++;
						continue;
					}
					else {
						patterns.push_back(std::make_pair(1, c));
						parsing_string = true; // 在解析模板字符时遇到%，表示这里是一个%转义
						// parsing_pattern = false;
						i++;
						continue;
					}
				}
				else { // not %
					if (parsing_string) { // 持续解析常规字符直到遇到%，解析出的字符串作为一个常规字符串加入patterns
						tmp += c;
						i++;
						continue;
					}
					else { // 模板字符，直接添加到patterns中，添加完成后，状态变为解析常规字符，%d特殊处理
						patterns.push_back(std::make_pair(1, c));
						parsing_string = true;
						// parsing_pattern = false;

						// 后面是对%d的特殊处理，如果%d后面直接跟了一对大括号，那么把大括号里面的内容提取出来作为dateformat
						if (c != "d") {
							i++;
							continue;
						}
						i++;
						if (i < m_pattern.size() && m_pattern[i] != '{') {
							continue;
						}
						i++;
						while (i < m_pattern.size() && m_pattern[i] != '}') {
							dateformat.push_back(m_pattern[i]);
							i++;
						}
						if (m_pattern[i] != '}') {
							// %d后面的大括号没有闭合，直接报错
							std::cout << "[ERROR] LogFormatter::init() " << "pattern: [" << m_pattern << "] '{' not closed" << std::endl;
							error = true;
							break;
						}
						i++;
						continue;
					}
				}
			} // end while(i < m_pattern.size())

			if (error) {
				m_error = true;
				return;
			}

			// 模板解析结束之后剩余的常规字符也要算进去
			if (!tmp.empty()) {
				patterns.push_back(std::make_pair(0, tmp));
				tmp.clear();
			}

			static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
				#define XX(str, C)  {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));} }
						XX(m, MessageFormatItem),           // m:消息
						XX(p, LevelFormatItem),             // p:日志级别
						XX(c, LoggerNameFormatItem),        // c:日志器名称
						XX(d, DateTimeFormatItem),          // d:日期时间
						XX(f, FileNameFormatItem),          // f:文件名
						XX(l, LineFormatItem),              // l:行号
						XX(t, ThreadIdFormatItem),          // t:线程号
						XX(N, SenderFormatItem),            // N:名称
						XX(%, PercentSignFormatItem),       // %:百分号
						XX(T, TabFormatItem),               // T:制表符
						XX(n, NewLineFormatItem),           // n:换行符
				#undef XX
			};

			for (auto& v : patterns) {
				if (v.first == 0) {
					m_items.push_back(FormatItem::ptr(new StringFormatItem(v.second)));
				}
				else if (v.second == "d") {
					m_items.push_back(FormatItem::ptr(new DateTimeFormatItem(dateformat)));
				}
				else {
					auto it = s_format_items.find(v.second);
					if (it == s_format_items.end()) {
						std::cout << "[ERROR] LogFormatter::init() " << "pattern: [" << m_pattern << "] " <<
							"unknown format item: " << v.second << std::endl;
						error = true;
						break;
					}
					else {
						m_items.push_back(it->second(v.second));
					}
				}
			}

			if (error) {
				m_error = true;
				return;
			}
		}

		std::string LogFormatter::format(LogEvent::ptr event) {
			std::stringstream ss;
			for (auto& i : m_items) {
				i->format(ss, event);
			}
			return ss.str();
		}

		std::ostream& LogFormatter::format(std::ostream& os, LogEvent::ptr event) {
			for (auto& i : m_items) {
				i->format(os, event);
			}
			return os;
		}
#pragma endregion

#pragma region Appender
		FileLogAppender::FileLogAppender(const std::string& file)
			: LogAppender(LogFormatter::ptr(new LogFormatter)) {
			m_filename = file;
			reopen();
			if (m_reopenError) {
				std::cout << "reopen file " << m_filename << " error" << std::endl;
			}
		}

		void FileLogAppender::log(LogEvent::ptr event) {
			uint64_t now = event->getTime();
			if (now >= (m_lastTime + 3)) {
				reopen();
				if (m_reopenError) {
					std::cout << "reopen file " << m_filename << " error" << std::endl;
				}
				m_lastTime = now;
			}
			if (m_reopenError) {
				return;
			}
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			if (m_formatter) {
				if (!m_formatter->format(m_filestream, event)) {
					std::cout << "[ERROR] FileLogAppender::log() format error" << std::endl;
				}
			}
			else {
				if (!m_defaultFormatter->format(m_filestream, event)) {
					std::cout << "[ERROR] FileLogAppender::log() format error" << std::endl;
				}
			}
		}

		bool FileLogAppender::reopen() {
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			if (m_filestream) {
				m_filestream.close();
			}
			m_filestream.open(m_filename, std::ios::app);
			m_reopenError = !m_filestream;
			return !m_reopenError;
		}
#pragma endregion

#pragma region Logger
		Logger::Logger(const std::string& name)
			: m_name(name)
			, m_level(LogLevel::Level::INFO)
			, m_createTime(Nano::Utils::GetElapsedMS()) {
		}

		void Logger::addAppender(LogAppender::ptr appender) {
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			m_appenders.push_back(appender);
		}

		void Logger::delAppender(LogAppender::ptr appender) {
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			for (auto it = m_appenders.begin(); it != m_appenders.end(); it++) {
				if (*it == appender) {
					m_appenders.erase(it);
					break;
				}
			}
		}

		void Logger::clearAppenders() {
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			m_appenders.clear();
		}

		void Logger::log(LogEvent::ptr event) {
			if (event->getLevel() <= m_level) {
				/// Logger至少要有一个appender，否则没有输出
				for (auto& i : m_appenders) {
					i->log(event);
				}
			}
		}
#pragma endregion

		Logger::ptr LoggerManager::getLogger(const std::string& name)
		{
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			auto it = m_loggers.find(name);
			if (it != m_loggers.end()) {
				return it->second;
			}

			Logger::ptr logger(new Logger(name));
			m_loggers[name] = logger;
			return logger;
		}

		AsyncLogger::AsyncLogger(const std::string& name) : m_name(name)
			, m_level(LogLevel::Level::INFO)
			, m_createTime(Nano::Utils::GetElapsedMS())
			, m_isExit(false)
		{
			m_thread = std::make_shared<std::thread>(std::bind(&AsyncLogger::threadFunc, this));
		}

		AsyncLogger::~AsyncLogger()
		{
			if (!m_isExit) {
				m_isExit.exchange(true);
				m_queue.Push(nullptr);
			}
			this->m_thread->join();
			std::cout << "Logger: " << m_name << " clear." << std::endl;
			this->m_queue.Clear();
			std::cout << "Logger: " << m_name << " exit." << std::endl;
		}

		void AsyncLogger::addAppender(LogAppender::ptr appender)
		{
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			m_appenders.push_back(appender);
		}

		void AsyncLogger::delAppender(LogAppender::ptr appender)
		{
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			for (auto it = m_appenders.begin(); it != m_appenders.end(); it++) {
				if (*it == appender) {
					m_appenders.erase(it);
					break;
				}
			}
		}

		void AsyncLogger::clearAppenders()
		{
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			m_appenders.clear();
		}

		void AsyncLogger::log(LogEvent::ptr event)
		{
			this->m_queue.Push(event);
		}

		void AsyncLogger::threadFunc()
		{
			while (!m_isExit.load()) {
				auto event = *(m_queue.WaitPop());
				if (event == nullptr) {
					m_isExit.exchange(true);
					break;
				}
				if (event->getLevel() <= m_level) {
					
					for (auto& i : m_appenders) {
						i->log(event);
					}
				}	
			}
			//std::cout << "AsyncLogger::threadFunc() exit" << std::endl;
		}

		AsyncLogger::ptr AsyncLoggerManager::getLogger(const std::string& name)
		{
			std::lock_guard<Nano::Mutex::SpinLock> lock(m_mutex);
			auto it = m_loggers.find(name);
			if (it != m_loggers.end()) {
				return it->second;
			}
			AsyncLogger::ptr logger(new AsyncLogger(name));
			m_loggers[name] = logger;
			return logger;
		}
	}
}