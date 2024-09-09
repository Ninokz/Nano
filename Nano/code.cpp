#include "code.h"

namespace Nano {
	namespace TransferCode {
		inline bool Code::encode(const std::string& str, char** buffer, size_t* len)
		{
			try {
				/// str -> buffer
				*len = str.size();
				*buffer = new char[*len];
				memcpy(*buffer, str.c_str(), *len);
				return true;
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				return false;
			}
		}

		inline std::string Code::decode(const char* buffer, size_t len)
		{
			try {
				/// buffer -> str
				std::string str(buffer, len);
				return str;
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				return "";
			}
		}
	}
}