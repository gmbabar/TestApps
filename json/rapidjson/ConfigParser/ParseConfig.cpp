// This code does not handle the exceptions

#include <iostream>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <rapidjson/istreamwrapper.h>
#include <boost/any.hpp>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <sstream>

using namespace rapidjson;

struct MD2ConfigParser {
    MD2ConfigParser(std::string &configFile) {
        this->ParseConfig(configFile);
    }

    std::string asStr(const std::string &key, const std::string &defaultVal="") {
        auto itr = m_values.find(key);
        if (itr != m_values.end() && itr->second.GetType() == ConfigValue::TypeString) {
            return boost::any_cast<std::string>(itr->second.GetValue());
        }
        return defaultVal;
    }

    int asInt(const std::string &key, const int &defaultVal=1) {
        auto itr = m_values.find(key);
        if (itr != m_values.end() && itr->second.GetType() == ConfigValue::TypeString) {
            return boost::any_cast<int>(itr->second.GetValue());
        }
        return defaultVal;
    }

    bool asBool(const std::string &key, const bool &defaultVal=false) {
        auto itr = m_values.find(key);
        if (itr != m_values.end() && itr->second.GetType() == ConfigValue::TypeString) {
            return boost::any_cast<bool>(itr->second.GetValue());
        }
        return defaultVal;
    }

    double asDouble(const std::string &key, const double &defaultVal=0.0) {
        auto itr = m_values.find(key);
        if (itr != m_values.end() && itr->second.GetType() == ConfigValue::TypeString) {
            return boost::any_cast<bool>(itr->second.GetValue());
        }
        return defaultVal;
    }
private:

    void ParseConfigEx(rapidjson::Value val, std::string prefix) {
        if (!prefix.empty())
            prefix += ".";
        for (auto& vitr : val.GetObject()) {
            // int type = vitr.value.GetType();
            if (vitr.value.IsObject()) {
                ParseConfigEx(vitr.value.GetObject(), prefix + vitr.name.GetString());
            }
            if (vitr.value.IsArray()) {
                auto arr = vitr.value.GetArray();
                for (SizeType i = 0; i < arr.Size(); i++) {
                    if(arr[i].IsObject()) {
                        ParseConfigEx(arr[i].GetObject(), prefix + vitr.name.GetString());
                    }
                }
            }
            else {
                m_values.emplace(prefix + vitr.name.GetString(), ConfigValue(vitr.value));
            }
        }
    }

    void ParseConfig(std::string &configFile) {
        std::ifstream Md2ConfigFile(configFile);
        rapidjson::IStreamWrapper json(Md2ConfigFile);
        rapidjson::Document document;
        document.ParseStream(json);

        if (document.IsObject()) {
            ParseConfigEx(document.GetObject(), "");
        }
    }

private:
    struct ConfigValue {
        enum ConfigType {
            TypeNull,
            TypeBool,
            TypeInt,
            TypeDouble,
            TypeString
        };

        // default ctor
        ConfigValue() = default;

        // ctor
        explicit ConfigValue(const rapidjson::Value &aVal) : m_type(TypeNull) {
            if (aVal.IsBool()) {
                m_type = TypeBool;
                m_value = aVal.GetBool();
            }
            else if (aVal.IsInt()) {
                m_type = TypeInt;
                m_value = aVal.GetInt();
            }
            else if (aVal.IsInt64()) {
                m_type = TypeInt;
                m_value = aVal.GetInt64();
            }
            else if (aVal.IsDouble()) {
                m_type = TypeDouble;
                m_value = aVal.GetDouble();
            }
            else if (aVal.IsString()) {
                m_type = TypeString;
                m_value = std::string(aVal.GetString());
            }
        }

        // copy ctor
        ConfigValue(const ConfigValue &other) {
            m_type = other.m_type;
            m_value = other.m_value;
        }

        // assignment operator
        void operator=(const ConfigValue &other) {
            m_type = other.m_type;
            m_value = other.m_value;
        }

    // getters
        boost::any GetValue() {
            return m_value;
        }

        ConfigType GetType() {
            return m_type;
        }

    private:
        ConfigType m_type;
        boost::any m_value;
    };

private:

    std::unordered_map<std::string, ConfigValue> m_values;
};


int main()
{
    std::string file = "./configfiles/MD2config.json";
    const std::string cmd = "CONNECTORS.configs.port";
    MD2ConfigParser parser(file);
    // std::cout << parser.asInt(cmd) << std::endl;
    // std::cout << parser.asStr("ROLE") << std::endl;
    // std::cout << parser.asStr("APP_ID") << std::endl;
    // std::cout << parser.asInt("APP_CODE") << std::endl;
    // std::cout << parser.asStr("LOGGING.log_level") << std::endl;
    // std::cout << parser.asStr("LOGGING.log_path") << std::endl;
    // std::cout << parser.asStr("LOGGING.destinations.log_type") << std::endl;
    // std::cout << parser.asStr("LOGGING.destinations.log_facility") << std::endl;
    // std::cout << parser.asStr("LOGGING.destinations.enabled") << std::endl;
    std::cout << "SYMBOLS.symbol_mapping: " << parser.asStr("SYMBOLS.symbol_mapping") << std::endl;
    std::cout << "SYMBOLS.secmaster_refdata_host: " << parser.asStr("SYMBOLS.secmaster_refdata_host") << std::endl;
    std::cout << "SYMBOLS.symbol_mapping: " << parser.asStr("SYMBOLS.symbol_mapping") << std::endl;
    std::cout << "SYMBOLS.secmaster_refdata_host: " << parser.asStr("SYMBOLS.secmaster_refdata_host") << std::endl;
    // std::cout << parser.asStr("SYMBOLS.secmaster_refdata_port") << std::endl;
    // std::cout << parser.asStr("SYMBOLS.secmaster_refdata_path") << std::endl;
    // std::cout << parser.asStr("SYMBOLS.secmaster_symbols_host") << std::endl;
    // std::cout << parser.asStr("SYMBOLS.secmaster_symbols_port") << std::endl;
    // std::cout << parser.asStr("SYMBOLS.secmaster_symbols_path") << std::endl;
    // std::cout << parser.asStr("SYMBOLS.exchange_symbol_prefix") << std::endl;
    // std::cout << parser.asStr("MD_SERVICE.exchange_symbol_prefix") << std::endl;

    return 0;
}