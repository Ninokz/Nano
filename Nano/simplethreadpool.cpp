#include "simplethreadpool.h"

namespace Nano {
	namespace Concurrency {
		SimpleThreadPool::SimpleThreadPool(int poolSize) : m_threadNum(poolSize), m_stop(false)
		{
			if (poolSize <= 1)
				m_threadNum = 2;
			else
				m_threadNum = poolSize;
			Start();
		}

		void SimpleThreadPool::Start()
		{
			for (int i = 0; i < m_threadNum; i++)
			{
				this->m_pool.emplace_back([this]()
					{
						while (!m_stop.load())
						{
							Task task;
							{
								std::unique_lock<std::mutex> lock(m_cv_mt);
								this->m_cv_lock.wait(lock, [this] {
									return this->m_stop.load() || !this->m_tasks.empty();
									});

								if (this->m_tasks.empty())
									return;

								task = std::move(this->m_tasks.front());
								this->m_tasks.pop();
							}
							this->m_threadNum--;
							task();
							this->m_threadNum++;
						}
					});
			}
		}

		void SimpleThreadPool::Stop()
		{
			m_stop.store(true);
			m_cv_lock.notify_all();
			for (auto& td : m_pool) {
				if (td.joinable()) {
					td.join();
				}
			}
		}

		SimpleThreadPool::~SimpleThreadPool()
		{
			Stop();
		}
	}
}