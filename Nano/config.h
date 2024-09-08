#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <list>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <sstream>


#include <json/json.h>
#include <boost/lexical_cast.hpp>

namespace Nano {
	namespace Config {
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
            virtual std::string toString() = 0;
            virtual bool fromString(const std::string& val) = 0;
            virtual std::string getTypeName() const = 0;
        protected:
            std::string m_name;
            std::string m_description;
        };

        template <class F, class T>
        class LexicalCast {
        public:
            T operator()(const F& v) {
                return boost::lexical_cast<T>(v);
            }
        };

      
	}
}