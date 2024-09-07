#include "utils.h"

namespace Nano {
	namespace Utils {
		const static uint64_t g_start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		uint64_t GetElapsedMS()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - g_start_time;
		}

		uint64_t GetThreadId()
		{
			return std::hash<std::thread::id>{}(std::this_thread::get_id());
		}
	}
}