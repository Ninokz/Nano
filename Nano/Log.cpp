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
			// ��˳��洢��������pattern��
			// ÿ��pattern����һ���������ͺ�һ���ַ���������Ϊ0��ʾ��pattern�ǳ����ַ�����Ϊ1��ʾ��pattern��Ҫת��
			// ���ڸ�ʽ�����������dataformat�洢
			std::vector<std::pair<int, std::string>> patterns;
			// ��ʱ�洢�����ַ���
			std::string tmp;
			// ���ڸ�ʽ�ַ�����Ĭ�ϰ�λ��%d����Ĵ����Ŷ����ȫ���ַ���������ʽ�ַ�����У���ʽ�Ƿ�Ϸ�
			std::string dateformat;
			// �Ƿ��������
			bool error = false;

			// �Ƿ����ڽ��������ַ�����ʼʱΪtrue
			bool parsing_string = true;
			// �Ƿ����ڽ���ģ���ַ���%�������ģ���ַ�
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
						parsing_string = false; // �ڽ��������ַ�ʱ����%����ʾ��ʼ����ģ���ַ�
						// parsing_pattern = true;
						i++;
						continue;
					}
					else {
						patterns.push_back(std::make_pair(1, c));
						parsing_string = true; // �ڽ���ģ���ַ�ʱ����%����ʾ������һ��%ת��
						// parsing_pattern = false;
						i++;
						continue;
					}
				}
				else { // not %
					if (parsing_string) { // �������������ַ�ֱ������%�����������ַ�����Ϊһ�������ַ�������patterns
						tmp += c;
						i++;
						continue;
					}
					else { // ģ���ַ���ֱ����ӵ�patterns�У������ɺ�״̬��Ϊ���������ַ���%d���⴦��
						patterns.push_back(std::make_pair(1, c));
						parsing_string = true;
						// parsing_pattern = false;

						// �����Ƕ�%d�����⴦�����%d����ֱ�Ӹ���һ�Դ����ţ���ô�Ѵ����������������ȡ������Ϊdateformat
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
							// %d����Ĵ�����û�бպϣ�ֱ�ӱ���
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

			// ģ���������֮��ʣ��ĳ����ַ�ҲҪ���ȥ
			if (!tmp.empty()) {
				patterns.push_back(std::make_pair(0, tmp));
				tmp.clear();
			}

			static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
				#define XX(str, C)  {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));} }
						XX(m, MessageFormatItem),           // m:��Ϣ
						XX(p, LevelFormatItem),             // p:��־����
						XX(c, LoggerNameFormatItem),        // c:��־������
						XX(d, DateTimeFormatItem),          // d:����ʱ��
						XX(f, FileNameFormatItem),          // f:�ļ���
						XX(l, LineFormatItem),              // l:�к�
						XX(t, ThreadIdFormatItem),          // t:�̺߳�
						XX(N, SenderFormatItem),            // N:����
						XX(%, PercentSignFormatItem),       // %:�ٷֺ�
						XX(T, TabFormatItem),               // T:�Ʊ��
						XX(n, NewLineFormatItem),           // n:���з�
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
				/// Logger����Ҫ��һ��appender������û�����
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