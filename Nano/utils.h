#pragma once
#include <string>
#include <memory>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>

namespace Nano {
	namespace Utils {
		uint64_t GetElapsedMS();

		uint64_t GetThreadId();
	}
}