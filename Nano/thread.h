#pragma once

#include <functional>
#include <string>
#include <sys/syscall.h>
#include "mutex.h"
#include "noncopyable.h"

using namespace Nano::Mutex;
namespace Nano {
	namespace Concurrency {
		class Thread : Noncopyable {
		public:
            Thread(std::function<void()> cb, const std::string& name);
            ~Thread();

            pid_t getId() const { return m_id; }
            const std::string& getName() const { return m_name; }
            void join();
            static Thread* GetThis();
            static const std::string& GetName();
            static void SetName(const std::string& name);
        private:
            static void* run(void* arg);
        private:
            pid_t m_id = -1;
            pthread_t m_thread;
            std::function<void()> m_cb;
            std::string m_name;
            Semaphore m_semaphore;
		};
	}
}