#pragma once
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#endif

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>

#include "singleton.h"

namespace Nano {
	namespace Env {
		class EnvMgr : public Singleton<EnvMgr> {
			friend class Singleton<EnvMgr>;
		private:
			EnvMgr();
		public:
			std::string GetRootPath() {
				return m_rootPath;
			}

			std::string GetConfigFolderPath() {
				return m_configFolderPath;
			}

			std::string GetLogFolderPath() {
				return m_logFolderPath;
			}

			static std::string GetCurrentDirectoryPlatformSpecific();

			void addEnvVar(const std::string& key, const std::string& value) {
				m_envVars[key] = value;
			}

			std::string getEnvVar(const std::string& key) {
				return m_envVars[key];
			}

			void delEnvVar(const std::string& key) {
				m_envVars.erase(key);
			}

		private:
			std::string m_rootPath;
			std::string m_configFolderPath;
			std::string m_logFolderPath;

			std::map<std::string, std::string> m_envVars;
		};
	}
}