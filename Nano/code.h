#pragma once
#include <string>
#include <iostream>

namespace Nano {
	namespace TransferCode {
		class Code
		{
		public:
			static inline bool encode(const std::string& str, char** buffer, int* len) {
				try {
					/// str -> buffer
					size_t length = str.size();
					if (length > INT32_MAX)
						throw std::runtime_error("encode: buffer size is too large");
					*len = static_cast<int>(length);
					*buffer = new char[length];
					memcpy(*buffer, str.c_str(), length);
					return true;
				}
				catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					return false;
				}
			}
			static inline std::string decode(const char* buffer, int len) {
				try {
					std::string str(buffer, len);
					return std::move(str);
				}
				catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					return "";
				}
			}
		};
	}
}