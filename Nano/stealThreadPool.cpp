#include "stealThreadPool.h"
#include <random>

namespace Nano {
	namespace Concurrency {
		StealThreadPool::StealThreadPool() :
			m_done(false), m_joiner(m_threads), m_atm_index(0), 
			m_fail_count_limit(Const::STEAL_THREAD_POOL_FAIL_COUNT_LIMIT), 
			m_max_backoff_time(Const::STEAL_THREAD_POOL_MAX_BACKOFF_TIME)
		{
			unsigned const thread_count = Const::THREAD_POOL_SIZE;
			try
			{
				m_thread_work_ques = std::vector <Concurrency::ConcurrentQueue<FunctionWrapper>>(thread_count);

				for (unsigned i = 0; i < thread_count; ++i)
				{
					m_threads.push_back(std::thread(&StealThreadPool::worker_thread, this, i));
				}
			}
			catch (...)
			{
				m_done = true;
				for (int i = 0; i < m_thread_work_ques.size(); i++) {
					m_thread_work_ques[i].Exit();
				}
				throw;
			}
		}

		void StealThreadPool::worker_thread(int index)
		{
			int fail_count = 0;
			int backoff_time_ms = 1;
			while (!m_done)
			{
				FunctionWrapper wrapper;
				if (m_thread_work_ques[index].try_pop(wrapper) || try_steal_from_others(index, wrapper)) {
					fail_count = 0;
					backoff_time_ms = 1; // 成功获取任务后，重置退避时间
					wrapper();
				}
				else {
					++fail_count;
					if (fail_count > m_fail_count_limit) {
						std::this_thread::sleep_for(std::chrono::milliseconds(backoff_time_ms));
						// 指数退避策略，最多到 m_max_backoff_time 上限
						backoff_time_ms = std::min(backoff_time_ms * 2, m_max_backoff_time);
						fail_count = 0;
					}
					else {
						std::this_thread::yield();
					}
				}
			}
		}

		StealThreadPool::~StealThreadPool()
		{
			m_done = true;
			for (unsigned i = 0; i < m_thread_work_ques.size(); i++) {
				m_thread_work_ques[i].Exit();
			}

			for (unsigned i = 0; i < m_threads.size(); ++i)
			{
				m_threads[i].join();
			}
		}

		bool StealThreadPool::try_steal_from_others(int selfIndex, FunctionWrapper& wrapper)
		{
			for (int i = 0; i < m_thread_work_ques.size(); ++i) {
				if (i != selfIndex && m_thread_work_ques[i].try_steal(wrapper)) {
					return true;
				}
			}
			return false;
		}
	}
}