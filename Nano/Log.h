#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <cstdarg>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <fstream>
#include <mutex>
#include <iostream>
#include <functional>
#include <list>
#include <map>

#include "utils.h"
#include "mutex.h"
#include "singleton.h"

namespace Nano {
	namespace Log {
        class Logger;
        class LogManager;

		class LogLevel {
		public:
			enum class Level
			{
                FATAL = 0,
                ERROR = 100,
                WARN = 200,
                INFO = 300,
                DEBUG = 400,
				UNKNOWN = 500
			};
			static const char* ToString(LogLevel::Level level);
			static LogLevel::Level FromString(const std::string& str);
		};

        class LogEvent {
        public:
            typedef std::shared_ptr<LogEvent> ptr;

            LogEvent(const std::string& logger_name, LogLevel::Level level,
                const char* file, int32_t line, uint64_t thread_id,
                time_t time, const std::string& sender);
            LogLevel::Level getLevel() const { return m_level; }
            std::string getContent() const { return m_ss.str(); }
            std::string getFile() const { return m_file; }
            int32_t getLine() const { return m_line; }
            uint64_t getThreadId() const { return m_threadId; }
            time_t getTime() const { return m_time; }
            const std::string& getSender() const { return m_sender; }
            std::stringstream& getSS() { return m_ss; }
            const std::string& getLoggerName() const { return m_loggerName; }

            void printf(const char* fmt, ...);
            void vprintf(const char* fmt, va_list ap);

        private:
            /// 文件名
            const char* m_file = nullptr;
            /// 行号
            int32_t m_line = 0;
            /// 线程id
            uint64_t m_threadId = 0;
            /// UTC时间戳
            time_t m_time;
            /// 日志级别
            LogLevel::Level m_level;
            /// 日志内容
            std::stringstream m_ss;
            /// 发送者名称
            std::string m_sender;
            /// 日志器名称
            std::string m_loggerName;
        };

        class LogFormatter {
        public:       
            typedef std::shared_ptr<LogFormatter> ptr;
            LogFormatter(const std::string& pattern = "%d{%Y-%m-%d %H:%M:%S} %T%t%T%N%T%T[%p]%T[%c]%T%f:%l%T%m%n");
            
            void init();
            bool isError() const { return m_error; }
            std::string format(LogEvent::ptr event);
            std::ostream& format(std::ostream& os, LogEvent::ptr event);
            std::string getPattern() const { return m_pattern; }

        public:
            class FormatItem {
            public:
                typedef std::shared_ptr<FormatItem> ptr;
                virtual ~FormatItem() {}
                virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
            };
        private:
            std::string m_pattern;
            std::vector<FormatItem::ptr> m_items;
            bool m_error = false;
        };

        class LogAppender {
        public:
            typedef std::shared_ptr<LogAppender> ptr;
            typedef Nano::Mutex::SpinLock MutexType;

			LogAppender(LogFormatter::ptr default_formatter) : m_defaultFormatter(default_formatter) {}
            virtual ~LogAppender() {}
            void setFormatter(LogFormatter::ptr val) {
				std::lock_guard<MutexType> lock(m_mutex);
				m_formatter = val;
            }

            LogFormatter::ptr getFormatter() {
                std::lock_guard<MutexType> lock(m_mutex);
                return m_formatter ? m_formatter : m_defaultFormatter;
            }
            virtual void log(LogEvent::ptr event) = 0;

        protected:
            MutexType m_mutex;
            LogFormatter::ptr m_formatter;
            LogFormatter::ptr m_defaultFormatter;
        };

        class StdoutLogAppender : public LogAppender {
        public:
            typedef std::shared_ptr<StdoutLogAppender> ptr;
			StdoutLogAppender() : LogAppender(std::make_shared<LogFormatter>()) {}
            void log(LogEvent::ptr event) override {
                if (m_formatter) {
                    m_formatter->format(std::cout, event);
                }
                else {
                    m_defaultFormatter->format(std::cout, event);
                }
            }
        };

        class FileLogAppender : public LogAppender {
        public:
            typedef std::shared_ptr<FileLogAppender> ptr;

            FileLogAppender(const std::string& file);
            void log(LogEvent::ptr event) override;
            bool reopen();
        private:
            /// 文件路径
            std::string m_filename;
            /// 文件流
            std::ofstream m_filestream;
            /// 上次重打打开时间
            uint64_t m_lastTime = 0;
            /// 文件打开错误标识
            bool m_reopenError = false;
        };

        class Logger {
        public:
            typedef std::shared_ptr<Logger> ptr;
            typedef Nano::Mutex::SpinLock MutexType;

            const std::string& getName() const { return m_name; }
            const uint64_t& getCreateTime() const { return m_createTime; }
            void setLevel(LogLevel::Level level) { m_level = level; }
            LogLevel::Level getLevel() const { return m_level; }

            Logger(const std::string& name);
            void addAppender(LogAppender::ptr appender);
            void delAppender(LogAppender::ptr appender);
            void clearAppenders();

            void log(LogEvent::ptr event);
        private:
            /// Mutex
            MutexType m_mutex;
            /// 日志器名称
            std::string m_name;
            /// 日志器等级
            LogLevel::Level m_level;
            /// LogAppender集合
            std::list<LogAppender::ptr> m_appenders;
            /// 创建时间（毫秒）
            uint64_t m_createTime;
        };

        class LogEventWrap {
        public:
            LogEventWrap(Logger::ptr logger, LogEvent::ptr event) : m_logger(logger) , 
                m_event(event) {
            }
			~LogEventWrap() { m_logger->log(m_event); }
            LogEvent::ptr getLogEvent() const { return m_event; }
        private:
            Logger::ptr m_logger;
            LogEvent::ptr m_event;
        };

        class LoggerManager {
        public:
            typedef Nano::Mutex::SpinLock MutexType;
            LoggerManager() {}
            Logger::ptr getLogger(const std::string& name);
        private:
            /// Mutex
            MutexType m_mutex;
            /// 日志器集合
            std::map<std::string, Logger::ptr> m_loggers;
        };

        typedef Nano::Singleton<LoggerManager> LoggerMgrSin;
    }

    #define LOG_MGR Log::LoggerMgrSin::GetInstance()
    #define LOG_NAME(name) LOG_MGR->getLogger(name)

    #define LOG_LEVEL(logger, level, sender) \
	    if (logger->getLevel() <= level) \
		    Nano::Log::LogEventWrap(logger, \
			    std::make_shared<Nano::Log::LogEvent>(logger->getName(), level, __FILE__, __LINE__, \
				    Nano::Utils::GetThreadId(), time(0), sender)).getLogEvent()->getSS()
    #define LOG_FATAL(logger,sender) LOG_LEVEL(logger, Nano::Log::LogLevel::Level::FATAL, sender)
    #define LOG_ERROR(logger,sender) LOG_LEVEL(logger, Nano::Log::LogLevel::Level::ERROR, sender)
    #define LOG_WARN(logger,sender) LOG_LEVEL(logger, Nano::Log::LogLevel::Level::WARN, sender)
    #define LOG_INFO(logger,sender) LOG_LEVEL(logger, Nano::Log::LogLevel::Level::INFO, sender)
    #define LOG_DEBUG(logger,sender) LOG_LEVEL(logger, Nano::Log::LogLevel::Level::DEBUG, sender)
    #define LOG_UNKNOWN(logger,sender) LOG_LEVEL(logger, Nano::Log::LogLevel::Level::UNKNOWN, sender)

    #define LOG_FMT_LEVEL(logger, level, sender, fmt, ...) \
	    if (logger->getLevel() <= level) \
		    Nano::Log::LogEventWrap(logger, \
			    std::make_shared<Nano::Log::LogEvent>(logger->getName(), level, __FILE__, __LINE__, \
				    Nano::Utils::GetThreadId(), time(0), sender)).getLogEvent()->printf(fmt, ##__VA_ARGS__)
    #define LOG_FMT_FATAL(logger,sender,fmt,...) LOG_FMT_LEVEL(logger, Nano::Log::LogLevel::Level::FATAL, sender, fmt, ##__VA_ARGS__)
    #define LOG_FMT_ERROR(logger,sender,fmt,...) LOG_FMT_LEVEL(logger, Nano::Log::LogLevel::Level::ERROR, sender, fmt, ##__VA_ARGS__)
    #define LOG_FMT_WARN(logger,sender,fmt,...) LOG_FMT_LEVEL(logger, Nano::Log::LogLevel::Level::WARN, sender, fmt, ##__VA_ARGS__)
    #define LOG_FMT_INFO(logger,sender,fmt,...) LOG_FMT_LEVEL(logger, Nano::Log::LogLevel::Level::INFO, sender, fmt, ##__VA_ARGS__)
    #define LOG_FMT_DEBUG(logger,sender,fmt,...) LOG_FMT_LEVEL(logger, Nano::Log::LogLevel::Level::DEBUG, sender, fmt, ##__VA_ARGS__)
    #define LOG_FMT_UNKNOWN(logger,sender,fmt,...) LOG_FMT_LEVEL(logger, Nano::Log::LogLevel::Level::UNKNOWN, sender, fmt, ##__VA_ARGS__)
}