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

struct MD2ConfigParser {
    MD2ConfigParser(const std::string &aConfigFile) {
        this->parseConfig(aConfigFile);
    }

    bool asBool(const std::string &aConfigKey, const bool &aDefaultVal=false) {
        auto itr = m_values.find(aConfigKey);
        if (itr != m_values.end() && itr->second.getType() == ConfigValue::TypeBool) {
            return boost::any_cast<bool>(itr->second.getValue());
        }
        return aDefaultVal;
    }

    int asInt(const std::string &aConfigKey, const int &aDefaultVal=0) {
        auto itr = m_values.find(aConfigKey);
        if (itr == m_values.end())
            return aDefaultVal;
        if (itr->second.getType() == ConfigValue::TypeString) {
            return std::stoi(boost::any_cast<std::string>(itr->second.getValue()));
        }
        if (itr->second.getType() == ConfigValue::TypeInt) {
            return boost::any_cast<int>(itr->second.getValue());
        }
        return aDefaultVal;
    }

    int asLong(const std::string &aConfigKey, const int &aDefaultVal=0) {
        auto itr = m_values.find(aConfigKey);
        if (itr == m_values.end())
            return aDefaultVal;
        if (itr->second.getType() == ConfigValue::TypeString) {
            return std::stol(boost::any_cast<std::string>(itr->second.getValue()));
        }
        if (itr->second.getType() == ConfigValue::TypeInt || itr->second.getType() == ConfigValue::TypeLong) {
            return boost::any_cast<long>(itr->second.getValue());
        }
        return aDefaultVal;
    }

    double asDouble(const std::string &aConfigKey, const double &aDefaultVal=0.0) {
        auto itr = m_values.find(aConfigKey);
        if (itr == m_values.end())
            return aDefaultVal;
        if (itr->second.getType() == ConfigValue::TypeString) {
            return std::stod(boost::any_cast<std::string>(itr->second.getValue()));
        }
        if (itr->second.getType() == ConfigValue::TypeDouble) {
            return boost::any_cast<double>(itr->second.getValue());
        }
        return aDefaultVal;
    }

    std::string asStr(const std::string &aConfigKey, const std::string &aDefaultVal="") {
        auto itr = m_values.find(aConfigKey);
        if (itr != m_values.end() && itr->second.getType() == ConfigValue::TypeString) {
            return boost::any_cast<std::string>(itr->second.getValue());
        }
        return aDefaultVal;
    }

    // nebula::dtm::time_duration asTimeDuration(const std::string &aConfigKey,
    //         const nebula::dtm::time_duration aDefaultValue = nebula::dtm::seconds(0)) {
    //     auto value = this->asInt(aConfigKey, 0);
    //     if (value != 0)
    //         return nebula::dtm::seconds(value);
    //     return aDefaultValue;
    // }

private:
    void parseConfigObject(rapidjson::Value aVal, std::string aPrefix) {
        if (!aPrefix.empty())
            aPrefix += ".";
        for (auto& vitr : aVal.GetObject()) {
            // int type = vitr.value.getType();
            if (vitr.value.IsObject()) {
                parseConfigObject(vitr.value.GetObject(), aPrefix + vitr.name.GetString());
            }
            if (vitr.value.IsArray()) {
                auto arr = vitr.value.GetArray();
                for (rapidjson::SizeType i = 0; i < arr.Size(); i++) {
                    if(arr[i].IsObject()) {
                        parseConfigObject(arr[i].GetObject(), aPrefix + vitr.name.GetString());
                    }
                }
            }
            else {
                m_values.emplace(aPrefix + vitr.name.GetString(), ConfigValue(vitr.value));
            }
        }
    }

    void parseConfig(const std::string &aConfigFile) {
        std::ifstream Md2ConfigFile(aConfigFile);
        rapidjson::IStreamWrapper json(Md2ConfigFile);
        rapidjson::Document document;
        document.ParseStream(json);

        if (document.IsObject()) {
            parseConfigObject(document.GetObject(), "");
        }
    }

private:
    struct ConfigValue {
        enum ConfigType {
            TypeNull,
            TypeBool,
            TypeInt,
            TypeLong,
            TypeDouble,
            TypeString
        };

        // default ctors
        ConfigValue() = default;
        ConfigValue(const ConfigValue&) = default;

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

        // assignment
        ConfigValue& operator=(const ConfigValue&) = default;

        // getters
        boost::any getValue() {
            return m_value;
        }

        ConfigType getType() {
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
    std::cout << "APP_CODE: " << parser.asInt("APP_CODE") << std::endl;
    std::cout << "MD_SERVICE.listen_port: " << parser.asInt("MD_SERVICE.listen_port") << std::endl;
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