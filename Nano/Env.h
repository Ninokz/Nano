#pragma once
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#endif

#include <string>
#include <iostream>
#include <cstdlib>

#include "singleton.h"

namespace Nano {
	namespace Env {
		std::string GetCurrentDirectoryPlatformSpecific();
		
		class EnvMgr : public Singleton<EnvMgr> {
			friend class Singleton<EnvMgr>;
		private:
			EnvMgr();
		public:
			std::string GetRootPath() {
				return m_rootPath;
			}
		private:
			std::string m_rootPath;
		};
	}
}