#include "stealThreadPool.h"

namespace Nano {
	namespace Concurrency {
		StealThreadPool::StealThreadPool() :
			m_done(false), m_joiner(m_threads), m_atm_index(0)
		{
			unsigned const thread_count = std::thread::hardware_concurrency();
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
			while (!m_done)
			{
				FunctionWrapper wrapper;
				bool pop_res = m_thread_work_ques[index].try_pop(wrapper);
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

				std::this_thread::yield();
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
	}
}