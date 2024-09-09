#pragma once
#include <string>
#include <memory>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <fstream>

#include "boost/filesystem.hpp"

#include "singleton.h"

namespace Nano {
	namespace Utils {
		uint64_t GetElapsedMS();

		uint64_t GetThreadId();
	}
}