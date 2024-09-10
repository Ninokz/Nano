#pragma once
#include <thread>
#include <vector>

#include "nocopyable.h"

namespace Nano {
	namespace Concurrency {
		class JoinThreads : public Noncopyable
        {
            std::vector<std::thread>& threads;
        public:
            explicit JoinThreads(std::vector<std::thread>& threads_) :
                threads(threads_) {}
            ~JoinThreads()
            {
                for (unsigned long i = 0; i < threads.size(); ++i)
                {
                    if (threads[i].joinable())
                        threads[i].join();
                }
            }
        };
	}
}