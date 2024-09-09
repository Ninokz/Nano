#pragma once
#include <string>
#include <iostream>

namespace Nano {
	namespace TransferCode {
		class Code
		{
		public:
			static inline bool encode(const std::string& str, char** buffer, size_t* len);
			static inline std::string decode(const char* buffer, size_t len);
		};
	}
}