#pragma once
#include <vector>
#include <memory>

#include <boost/asio.hpp>

#include "singleton.h"
#include "nocopyable.h"

namespace Nano {
	namespace Communication {
		class IOServicePool : public Singleton<IOServicePool>
		{
			friend class Singleton<IOServicePool>;
		public:
			using Context = boost::asio::io_context;
			using Work = boost::asio::io_context::work;
			using WorkPtr = std::unique_ptr<boost::asio::io_context::work>;

			virtual ~IOServicePool();
			Context& getIOContext();
		private:
			IOServicePool(std::size_t poolSize = std::thread::hardware_concurrency());
			void Stop();

			IOServicePool(const IOServicePool&) = delete;
			IOServicePool& operator=(const IOServicePool&) = delete;
			IOServicePool(IOServicePool&&) = delete;
		private:
			std::vector<Context> m_ioContexts;
			std::vector<WorkPtr> m_works;
			std::vector<std::thread> m_threads;
			std::size_t m_nextIOContext;
		};
	}
}
