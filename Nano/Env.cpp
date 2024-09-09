#include "env.h"

namespace Nano {
	namespace Env {
		EnvMgr::EnvMgr()
		{
			m_rootPath = GetCurrentDirectoryPlatformSpecific();
			m_configFolderPath = m_rootPath + "\\config";
			m_logFolderPath = m_rootPath + "\\logs";
		}

        std::string EnvMgr::GetCurrentDirectoryPlatformSpecific()
        {
#ifdef _WIN32
            wchar_t cwd[MAX_PATH];
            DWORD length = GetCurrentDirectory(MAX_PATH, cwd);
            if (length > 0 && length < MAX_PATH) {
                char* cstr = new char[MAX_PATH];
                size_t convertedChars = 0;
                errno_t err = wcstombs_s(&convertedChars, cstr, MAX_PATH, cwd, _TRUNCATE);
                if (err != 0) {
                    delete[] cstr;
                    throw std::runtime_error("wcstombs_s failed");
                }
                else {
                    std::string str(cstr);
                    delete[] cstr;
                    return str;
                }
            }
            else {
                throw std::runtime_error("GetCurrentDirectory failed or buffer too small");
            }
#elif __linux__
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                return std::string(cwd);
            }
            else {
                throw std::runtime_error("getcwd() failed on Linux");
            }
#endif
        }
	}
}