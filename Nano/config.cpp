#include "config.h"

namespace Nano {
	namespace Configuration {
		void Config::LoadFromConfDir(const std::string& path) {
			namespace fs = boost::filesystem;

			// 检查路径是否存在且为目录
			if (!fs::exists(path) || !fs::is_directory(path)) {
				std::cerr << "Config::LoadFromConfDir: Path does not exist or is not a directory: " << path << std::endl;
				return;
			}

			fs::directory_iterator end_iter;
			for (fs::directory_iterator it(path); it != end_iter; ++it) {
				if (fs::is_regular_file(it->path()) && it->path().extension() == ".json") {
					std::ifstream file(it->path().string());
					if (!file.is_open()) {
						std::cerr << "Config::LoadFromConfDir: Unable to open file: " << it->path().string() << std::endl;
						continue;
					}
					Json::Value root;
					Json::CharReaderBuilder builder;
					std::string errs;

					// 解析 JSON 文件内容
					if (!Json::parseFromStream(builder, file, &root, &errs)) {
						std::cerr << "Config::LoadFromConfDir: Failed to parse JSON from file: "
							<< it->path().string() << "\nError: " << errs << std::endl;
						continue;
					}

					// 遍历 JSON 对象中的键值对
					for (const auto& key : root.getMemberNames()) {
						Json::Value value = root[key];
						if (value.isInt()) {
							int val = value.asInt();
							ConfigVar<int> configVar(key, val);
							if (Config::Lookup<int>(key) == nullptr)
								Config::Register<int>(key, val);
						}
						else if (value.isString()) {
							std::string val = value.asString();
							ConfigVar<std::string> configVar(key, val);
							if (Config::Lookup<std::string>(key) == nullptr)
								Config::Register<std::string>(key, val);
						}
						else if (value.isArray())
						{
							if (value.size() == 0)
							{
								std::cerr << "Config::LoadFromConfDir: Empty array" << std::endl;
								continue;
							}
							if (Config::checkJsonArrayItemConsistency<std::string>(value))
							{
								std::vector<std::string> val;
								for (const auto& item : value)
									val.push_back(item.asString());
								ConfigVar<std::vector<std::string>> configVar(key, val);
								if (Config::Lookup<std::vector<std::string>>(key) == nullptr)
									Config::Register<std::vector<std::string>>(key, val);
							}
							else if (Config::checkJsonArrayItemConsistency<int>(value))
							{
								std::vector<int> val;
								for (const auto& item : value)
									val.push_back(item.asInt());
								ConfigVar<std::vector<int>> configVar(key, val);
								if (Config::Lookup<std::vector<int>>(key) == nullptr)
									Config::Register<std::vector<int>>(key, val);
							}
						}
						else
							std::cerr << "Config::LoadFromConfDir: Unsupported JSON type" << std::endl;
					}
				}
			}
		}
	}
}