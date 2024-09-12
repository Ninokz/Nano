#pragma once
#include <string>
#include <iostream>

namespace Nano {
	namespace TransferCode {
		class Code
		{
		public:
			static inline bool encode(const std::string& str, char** buffer, size_t* len) {
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

			static inline std::string decode(const char* buffer, size_t len) {
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