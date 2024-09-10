#pragma once

#include <map>
#include <set>
#include <list>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include <json/json.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "utils.h"

namespace Nano {
	namespace Configuration{
		class ConfigVarBase {
		public:
			typedef std::shared_ptr<ConfigVarBase> ptr;
			ConfigVarBase(const std::string& name, const std::string& description = "")
				: m_name(name)
				, m_description(description) {
				std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
			}
			virtual ~ConfigVarBase() {}
			const std::string& getName() const { return m_name; }
			const std::string& getDescription() const { return m_description; }
			void setDescription(const std::string& val) { m_description = val; }
			virtual std::string toString() = 0;
			virtual bool fromString(const std::string& val) = 0;
			virtual std::string getTypeName() const = 0;
		protected:
			std::string m_name;
			std::string m_description;
		};

#pragma region CAST
		template <class F, class T>
		class LexicalCast {
		public:
			T operator()(const F& v) {
				return boost::lexical_cast<T>(v);
			}
		};

#pragma region VECTOR CAST

		// 特化: 从 std::string 转换为 std::vector<T>，假设字符串为 JSON 数组格式
		template <class T>
		class LexicalCast<std::string, std::vector<T>> {
		public:
			std::vector<T> operator()(const std::string& v) {
				std::vector<T> vec;

				// 使用 Jsoncpp 库解析字符串为 JSON 对象
				Json::Value root;
				Json::CharReaderBuilder builder;
				std::string errs;

				std::istringstream ss(v);
				if (!Json::parseFromStream(builder, ss, &root, &errs)) {
					throw std::runtime_error("Failed to parse JSON: " + errs);
				}

				if (!root.isArray()) {
					throw std::runtime_error("Expected JSON array");
				}

				// 将 JSON 数组转换为 std::vector<T>
				for (const auto& item : root) {
					vec.push_back(LexicalCast<Json::Value, T>()(item));  // 假设存在 Json::Value -> T 的 LexicalCast
				}

				return vec;
			}
		};

		// 特化: 从 std::vector<T> 转换为 std::string，转换为 JSON 字符串
		template <class T>
		class LexicalCast<std::vector<T>, std::string> {
		public:
			std::string operator()(const std::vector<T>& v) {
				Json::Value root(Json::arrayValue);

				// 将 std::vector<T> 转换为 JSON 数组
				for (const auto& item : v) {
					root.append(LexicalCast<T, Json::Value>()(item));  // 假设存在 T -> Json::Value 的 LexicalCast
				}

				// 将 JSON 对象转换为字符串
				Json::StreamWriterBuilder writer;
				return Json::writeString(writer, root);
			}
		};

		// 特化: 从 Json::Value 转换为 std::string
		template <>
		class LexicalCast<Json::Value, std::string> {
		public:
			std::string operator()(const Json::Value& v) {
				return v.asString();
			}
		};

		// 特化: 从 std::string 转换为 Json::Value
		template <>
		class LexicalCast<std::string, Json::Value> {
		public:
			Json::Value operator()(const std::string& v) {
				return Json::Value(v);
			}
		};

		// 特化: 从 Json::Value 转换为 int
		template <>
		class LexicalCast<Json::Value, int> {
		public:
			int operator()(const Json::Value& v) {
				return v.asInt();
			}
		};

		// 特化: 从 int 转换为 Json::Value
		template <>
		class LexicalCast<int, Json::Value> {
		public:
			Json::Value operator()(const int& v) {
				return Json::Value(v);
			}
		};
#pragma endregion

#pragma region LIST CAST

		// 特化: 从 std::string 转换为 std::list<T>，假设字符串为 JSON 数组格式
		template <class T>
		class LexicalCast<std::string, std::list<T>> {
		public:
			std::list<T> operator()(const std::string& v) {
				std::list<T> lst;

				// 使用 Jsoncpp 库解析字符串为 JSON 对象
				Json::Value root;
				Json::CharReaderBuilder builder;
				std::string errs;

				std::istringstream ss(v);
				if (!Json::parseFromStream(builder, ss, &root, &errs)) {
					throw std::runtime_error("Failed to parse JSON: " + errs);
				}

				if (!root.isArray()) {
					throw std::runtime_error("Expected JSON array");
				}

				// 将 JSON 数组转换为 std::list<T>
				for (const auto& item : root) {
					lst.push_back(LexicalCast<Json::Value, T>()(item));  // 假设存在 Json::Value -> T 的 LexicalCast
				}

				return lst;
			}
		};

		// 特化: 从 std::list<T> 转换为 std::string，转换为 JSON 字符串
		template <class T>
		class LexicalCast<std::list<T>, std::string> {
		public:
			std::string operator()(const std::list<T>& v) {
				Json::Value root(Json::arrayValue);

				// 将 std::list<T> 转换为 JSON 数组
				for (const auto& item : v) {
					root.append(LexicalCast<T, Json::Value>()(item));  // 假设存在 T -> Json::Value 的 LexicalCast
				}

				// 将 JSON 对象转换为字符串
				Json::StreamWriterBuilder writer;
				return Json::writeString(writer, root);
			}
		};

#pragma endregion

#pragma region SET CAST
		// 从 std::string 转换为 std::set<T>，假设字符串为 JSON 数组格式
		template <class T>
		class LexicalCast<std::string, std::set<T>> {
		public:
			std::set<T> operator()(const std::string& v) {
				std::set<T> setResult;

				// 使用 jsoncpp 解析字符串为 JSON 对象
				Json::Value root;
				Json::CharReaderBuilder builder;
				std::string errs;

				std::istringstream ss(v);
				if (!Json::parseFromStream(builder, ss, &root, &errs)) {
					throw std::runtime_error("Failed to parse JSON: " + errs);
				}

				if (!root.isArray()) {
					throw std::runtime_error("Expected JSON array");
				}

				// 将 JSON 数组中的元素转换为 set<T>
				for (const auto& item : root) {
					setResult.insert(LexicalCast<Json::Value, T>()(item));  // 假设存在 Json::Value -> T 的 LexicalCast
				}

				return setResult;
			}
		};

		// 从 std::set<T> 转换为 std::string，转换为 JSON 字符串
		template <class T>
		class LexicalCast<std::set<T>, std::string> {
		public:
			std::string operator()(const std::set<T>& v) {
				Json::Value root(Json::arrayValue);

				// 将 std::set<T> 转换为 JSON 数组
				for (const auto& item : v) {
					root.append(LexicalCast<T, Json::Value>()(item));  // 假设存在 T -> Json::Value 的 LexicalCast
				}

				// 将 JSON 对象转换为字符串
				Json::StreamWriterBuilder writer;
				return Json::writeString(writer, root);
			}
		};
#pragma endregion

#pragma region unoreder_set cast
		template <class T>
		class LexicalCast<std::string, std::unordered_set<T>> {
		public:
			std::unordered_set<T> operator()(const std::string& v) {
				std::unordered_set<T> setResult;

				// 使用 jsoncpp 解析字符串为 JSON 对象
				Json::Value root;
				Json::CharReaderBuilder builder;
				std::string errs;

				std::istringstream ss(v);
				if (!Json::parseFromStream(builder, ss, &root, &errs)) {
					throw std::runtime_error("Failed to parse JSON: " + errs);
				}

				if (!root.isArray()) {
					throw std::runtime_error("Expected JSON array");
				}

				// 将 JSON 数组中的元素转换为 unordered_set<T>
				for (const auto& item : root) {
					setResult.insert(LexicalCast<Json::Value, T>()(item));  // 假设存在 Json::Value -> T 的 LexicalCast
				}

				return setResult;
			}
		};

		// 从 std::unordered_set<T> 转换为 std::string，转换为 JSON 字符串
		template <class T>
		class LexicalCast<std::unordered_set<T>, std::string> {
		public:
			std::string operator()(const std::unordered_set<T>& v) {
				Json::Value root(Json::arrayValue);

				// 将 std::unordered_set<T> 转换为 JSON 数组
				for (const auto& item : v) {
					root.append(LexicalCast<T, Json::Value>()(item));  // 假设存在 T -> Json::Value 的 LexicalCast
				}

				// 将 JSON 对象转换为字符串
				Json::StreamWriterBuilder writer;
				return Json::writeString(writer, root);
			}
		};
#pragma endregion

#pragma region map cast
		// 从 std::string 转换为 std::map<std::string, T>，假设字符串为 JSON 对象格式
		template <class T>
		class LexicalCast<std::string, std::map<std::string, T>> {
		public:
			std::map<std::string, T> operator()(const std::string& v) {
				std::map<std::string, T> mapResult;

				// 使用 jsoncpp 解析字符串为 JSON 对象
				Json::Value root;
				Json::CharReaderBuilder builder;
				std::string errs;

				std::istringstream ss(v);
				if (!Json::parseFromStream(builder, ss, &root, &errs)) {
					throw std::runtime_error("Failed to parse JSON: " + errs);
				}

				if (!root.isObject()) {
					throw std::runtime_error("Expected JSON object");
				}

				// 将 JSON 对象中的键值对转换为 std::map<std::string, T>
				for (const auto& key : root.getMemberNames()) {
					mapResult[key] = LexicalCast<Json::Value, T>()(root[key]);  // 假设存在 Json::Value -> T 的 LexicalCast
				}

				return mapResult;
			}
		};

		// 从 std::map<std::string, T> 转换为 std::string，转换为 JSON 字符串
		template <class T>
		class LexicalCast<std::map<std::string, T>, std::string> {
		public:
			std::string operator()(const std::map<std::string, T>& v) {
				Json::Value root(Json::objectValue);

				// 将 std::map<std::string, T> 转换为 JSON 对象
				for (const auto& pair : v) {
					root[pair.first] = LexicalCast<T, Json::Value>()(pair.second);  // 假设存在 T -> Json::Value 的 LexicalCast
				}

				// 将 JSON 对象转换为字符串
				Json::StreamWriterBuilder writer;
				return Json::writeString(writer, root);
			}
		};
#pragma endregion

#pragma region unordered_map cast

		// 从 std::string 转换为 std::unordered_map<std::string, T>，假设字符串为 JSON 对象格式
		template <class T>
		class LexicalCast<std::string, std::unordered_map<std::string, T>> {
		public:
			std::unordered_map<std::string, T> operator()(const std::string& v) {
				std::unordered_map<std::string, T> mapResult;

				// 使用 jsoncpp 解析字符串为 JSON 对象
				Json::Value root;
				Json::CharReaderBuilder builder;
				std::string errs;

				std::istringstream ss(v);
				if (!Json::parseFromStream(builder, ss, &root, &errs)) {
					throw std::runtime_error("Failed to parse JSON: " + errs);
				}

				if (!root.isObject()) {
					throw std::runtime_error("Expected JSON object");
				}

				// 将 JSON 对象中的键值对转换为 std::unordered_map<std::string, T>
				for (const auto& key : root.getMemberNames()) {
					mapResult[key] = LexicalCast<Json::Value, T>()(root[key]);  // 假设存在 Json::Value -> T 的 LexicalCast
				}

				return mapResult;
			}
		};

		// 从 std::unordered_map<std::string, T> 转换为 std::string，转换为 JSON 字符串
		template <class T>
		class LexicalCast<std::unordered_map<std::string, T>, std::string> {
		public:
			std::string operator()(const std::unordered_map<std::string, T>& v) {
				Json::Value root(Json::objectValue);

				// 将 std::unordered_map<std::string, T> 转换为 JSON 对象
				for (const auto& pair : v) {
					root[pair.first] = LexicalCast<T, Json::Value>()(pair.second);  // 假设存在 T -> Json::Value 的 LexicalCast
				}

				// 将 JSON 对象转换为字符串
				Json::StreamWriterBuilder writer;
				return Json::writeString(writer, root);
			}
		};
#pragma endregion

#pragma endregion

		template <class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
		class ConfigVar : public ConfigVarBase {
		public:
			typedef std::shared_ptr<ConfigVar> ptr;

			ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
				: ConfigVarBase(name, description)
				, m_val(default_value) {}

			std::string toString() override {
				try {
					return ToStr()(m_val);
				}
				catch (const std::exception& e) {
					std::cerr << "ConfigVar::toString exception: " << e.what() << std::endl;
					return "";
				}
			}

			bool fromString(const std::string& val) override {
				try {
					setValue(FromStr()(val));
					return true;
				}
				catch (const std::exception& e) {
					std::cerr << "ConfigVar::fromString exception: " << e.what() << std::endl;
				}
				return false;
			}

			const T getValue() const { return m_val; }

			void setValue(const T& v) {
				if (v == m_val) {
					return;
				}
				m_val = v;
			}

			std::string getTypeName() const override { return typeid(T).name(); }

		private:
			T m_val;
			typedef T value_type;
		};

		class Config : public Singleton<Config>
		{
			friend class Singleton<Config>;
		public:
			typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
			static void LoadFromConfDir(const std::string& path);
			template <class T>
			static bool checkJsonArrayItemConsistency(const Json::Value& root) {
				for (const auto& item : root) {
					if (!item.is<T>()) {
						std::cerr << "Config::LoadFromConfDir: Inconsistent JSON array item type" << std::endl;
						return false;
					}
				}
				return true;
			}

			template <class T>
			static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
				auto it = GetDatas().find(name);
				if (it == GetDatas().end()) {
					return nullptr;
				}
				return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
			}

			template <class T>
			static void Register(const std::string& name, const T& default_value, const std::string& description = "") {
				auto it = GetDatas().find(name);
				if (it != GetDatas().end()) {
					std::cerr << "Config::Register: ConfigVar " << name << " already exists" << std::endl;
					return;
				}
				GetDatas()[name] = std::make_shared<ConfigVar<T>>(name, default_value, description);
			}

			static void PrintAllConfigVars() {
				for (const auto& pair : GetDatas()) {
					std::cout << pair.first << " = " << pair.second->toString() << std::endl;
				}
			}

		public:
			static ConfigVarMap& GetDatas() {
				static ConfigVarMap s_configVarMap;
				return s_configVarMap;
			}
		};
	}
}