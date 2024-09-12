#pragma once
#include <atomic>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include "nocopyable.h"

#include "singleton.h"
#include "concurrentqueue.h"
#include "functionWrapper.h"
#include "jointhreads.h"
#include "const.h"

namespace Nano {
	namespace Concurrency {
		class ParrallenThreadPool : public Singleton<ParrallenThreadPool>, public Noncopyable
		{
			friend class Singleton<ParrallenThreadPool>;
		private:
			void worker_thread(int index);

			ParrallenThreadPool();
		public:
			template<typename FunctionType>
			std::future<typename std::result_of<FunctionType()>::type>
				submit(FunctionType f)
			{
				int index = (m_atm_index.load() + 1) % m_thread_work_ques.size();
				m_atm_index.store(index);
				typedef typename std::result_of<FunctionType()>::type result_type;
				std::packaged_task<result_type()> task(std::move(f));
				std::future<result_type> res(task.get_future());
				m_thread_work_ques[index].push(std::move(task));
				return res;
			}

			~ParrallenThreadPool();
		private:
			std::atomic_bool m_done;
			std::atomic<int> m_atm_index;
			std::vector<Concurrency::ConcurrentQueue<FunctionWrapper>> m_thread_work_ques;
			std::vector<std::thread> m_threads;
			JoinThreads m_joiner;
		};
	}
}