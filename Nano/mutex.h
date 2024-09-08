#pragma once
#include <atomic>

#include "nocopyable.h"

namespace Nano {
	namespace Mutex {
		class SpinLock {
			std::atomic_flag flag = ATOMIC_FLAG_INIT;
		public:
			SpinLock() = default;
			SpinLock(const SpinLock&) = delete;
			SpinLock& operator= (const SpinLock&) = delete;
			void lock() {
				while (flag.test_and_set(std::memory_order_acquire))
					;
			}

			void unlock() {
				flag.clear(std::memory_order_release);
			}
		};
	}
}
