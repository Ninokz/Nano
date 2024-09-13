#include "stealThreadPool.h"
#include <random>

namespace Nano {
	namespace Concurrency {
		StealThreadPool::StealThreadPool() :
			m_done(false), m_joiner(m_threads), m_atm_index(0)
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
			while (!m_done)
			{
				/// 从其他线程窃取任务
				FunctionWrapper wrapper;
				/// 忙等
				/*bool pop_res = m_thread_work_ques[index].try_pop(wrapper);
				if (pop_res) {
					wrapper();
					continue;
				}
				bool steal_res = false;
				for (int i = 0; i < m_thread_work_ques.size(); i++) {
					if (i == index) {
						continue;
					}
					steal_res = m_thread_work_ques[i].try_steal(wrapper);
					if (steal_res) {
						wrapper();
						break;
					}
				}
				if (steal_res) {
					continue;
				}
				std::this_thread::yield();*/

				/// 优化忙等 - 引入退避策略
				if (m_thread_work_ques[index].try_pop(wrapper) || try_steal_from_others(index,wrapper)) {
					fail_count = 0;
					wrapper();
				}
				else {
					++fail_count;
					if (fail_count > m_fail_count_limit) {
						std::this_thread::sleep_for(std::chrono::milliseconds(m_sleep_time));
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