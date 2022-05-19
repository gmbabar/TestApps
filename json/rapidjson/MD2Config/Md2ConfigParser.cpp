#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <boost/algorithm/string.hpp>
#include "Md2ExchContract.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <vector>
#include <map>
#include "boost/date_time/posix_time/posix_time.hpp" 

struct Md2ConfigParser {

    explicit Md2ConfigParser(const std::string &fileName) {
        std::ifstream fileStream(fileName);
        std::stringstream buffer;
        if (!fileStream.is_open()) {
            buffer << "Couldn't open configuration file: " << fileName;
            std::cout << "Error: " << buffer.str() << std::endl;
            throw std::runtime_error(buffer.str());
        } 
        buffer << fileStream.rdbuf();
        m_jsonStr = buffer.str();
        m_doc.Parse(m_jsonStr.c_str());
    }

    const rapidjson::Value* getObject(const std::string &aKey, const rapidjson::Value *aValuePtr = nullptr) {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = (aValuePtr != nullptr ? aValuePtr : &m_doc);
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {
                // std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return nullptr;
                levelVal = &val[aConfigKey.c_str()];
            }
            if (levelVal->IsObject())
                return levelVal;
            return nullptr;
        }

        const rapidjson::Value &jsonVal = (aValuePtr != nullptr ? (*aValuePtr) : m_doc);
        if ( jsonVal.IsObject() && jsonVal.HasMember(aKey.c_str()) && jsonVal[aKey.c_str()].IsObject() ) {
            return &jsonVal[aKey.c_str()];
        }
        return nullptr;
    }

    const rapidjson::Value* getArray(const std::string &aKey, const rapidjson::Value *aValuePtr = nullptr) {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = (aValuePtr != nullptr ? aValuePtr : &m_doc);
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {
                // std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return nullptr;
                levelVal = &val[aConfigKey.c_str()];
            }
            if (levelVal->IsArray())
                return levelVal;
            return nullptr;
        }

        const rapidjson::Value &jsonVal = (aValuePtr != nullptr ? (*aValuePtr) : m_doc);
        if ( jsonVal.IsObject() && jsonVal.HasMember(aKey.c_str()) && jsonVal[aKey.c_str()].IsArray() ) {
            return &jsonVal[aKey.c_str()];
        }
        return nullptr;
    }

    const std::string asStr(const std::string &aKey, const rapidjson::Value *aValuePtr = nullptr, const std::string &aDefaultValue = "") {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = (aValuePtr != nullptr ? aValuePtr : &m_doc);
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {
                // std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return aDefaultValue;
                levelVal = &val[aConfigKey.c_str()];
            }
            if (levelVal->IsString())
                return levelVal->GetString();
            return aDefaultValue;
        }

        const rapidjson::Value &jsonVal = (aValuePtr != nullptr ? *aValuePtr : m_doc);
        if ( jsonVal.IsObject() && jsonVal.HasMember(aKey.c_str()) && jsonVal[aKey.c_str()].IsString() ) {
            return jsonVal[aKey.c_str()].GetString();
        }
        return aDefaultValue;
    }

    int asInt(const std::string &aKey, const rapidjson::Value *aValuePtr = nullptr, int aDefaultValue = 0) {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = (aValuePtr != nullptr ? aValuePtr : &m_doc);
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {
                // std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return aDefaultValue;
                levelVal = &val[aConfigKey.c_str()];
            }
            if (levelVal->IsInt())
                return levelVal->GetInt();
            return aDefaultValue;
        }

        const rapidjson::Value &jsonVal = (aValuePtr != nullptr ? *aValuePtr : m_doc);
        if ( jsonVal.IsObject() && jsonVal.HasMember(aKey.c_str()) && jsonVal[aKey.c_str()].IsInt() ) {
            return jsonVal[aKey.c_str()].GetInt();
        }
        return aDefaultValue;
    }

    long asLong(const std::string &aKey, const rapidjson::Value *aValuePtr = nullptr, long aDefaultValue = 0) {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = (aValuePtr != nullptr ? aValuePtr : &m_doc);
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {
                // std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return aDefaultValue;
                levelVal = &val[aConfigKey.c_str()];
            }
            if (levelVal->IsInt64())
                return levelVal->GetInt64();
            return aDefaultValue;
        }

        const rapidjson::Value &jsonVal = (aValuePtr != nullptr ? *aValuePtr : m_doc);
        if ( jsonVal.IsObject() && jsonVal.HasMember(aKey.c_str()) && jsonVal[aKey.c_str()].IsInt64() ) {
            return jsonVal[aKey.c_str()].GetInt64();
        }
        return aDefaultValue;
    }

    double asDouble(const std::string &aKey, const rapidjson::Value *aValuePtr = nullptr, double aDefaultValue = 0) {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = (aValuePtr != nullptr ? aValuePtr : &m_doc);
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {
                // std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return aDefaultValue;
                levelVal = &val[aConfigKey.c_str()];
            }
            if (levelVal->IsDouble())
                return levelVal->GetDouble();
            return aDefaultValue;
        }

        const rapidjson::Value &jsonVal = (aValuePtr != nullptr ? *aValuePtr : m_doc);
        if ( jsonVal.IsObject() && jsonVal.HasMember(aKey.c_str()) && jsonVal[aKey.c_str()].IsDouble() ) {
            return jsonVal[aKey.c_str()].GetDouble();
        }
        return aDefaultValue;
    }

    bool asBool(const std::string &aKey, const rapidjson::Value *aValuePtr = nullptr, bool aDefaultValue = false) {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = (aValuePtr != nullptr ? aValuePtr : &m_doc);
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {
                // std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return aDefaultValue;
                levelVal = &val[aConfigKey.c_str()];
            }
            if (levelVal->IsBool())
                return levelVal->GetBool();
            return aDefaultValue;
        }

        const rapidjson::Value &jsonVal = (aValuePtr != nullptr ? *aValuePtr : m_doc);
        if ( jsonVal.IsObject() && jsonVal.HasMember(aKey.c_str()) && jsonVal[aKey.c_str()].IsBool() ) {
            return jsonVal[aKey.c_str()].GetBool();
        }
        return aDefaultValue;
    }

    // TODO: future implementation
    // bool asTimeDuration(const std::string &aKey, bool aDefaultValue = false) {
    //     if ( m_doc.HasMember(aKey.c_str()) && m_doc[aKey.c_str()].IsString() ) {
    //         return m_doc[aKey.c_str()].GetString();
    //     }
    //     return aDefaultValue;
    // }


private:
    rapidjson::Document m_doc;
    std::string m_jsonStr;
};

enum Md2ConfigType {
    TypeArray,
    TypeObject,
    TypeString,
    TypeInt,
    TypeLong,
    TypeDouble,
    TypeBool,
    TypeConstant,
    TypeDefault
};

struct Md2ConfigRuleItem {
    Md2ConfigRuleItem(std::string name, Md2ConfigType type) :
        m_configName(name), m_configType(type) {}
    
    Md2ConfigRuleItem(const Md2ConfigRuleItem& rule) :
        m_configName(rule.m_configName), m_configType(rule.m_configType) {}

    const std::string& configName() const {
        return m_configName;
    }
    
    const Md2ConfigType& configType() const {
        return m_configType;
    }
    
private:
    std::string m_configName;
    Md2ConfigType m_configType;
};

struct Md2ConfigMapper {
    explicit Md2ConfigMapper (const std::string &aFileName) : m_parser(aFileName) {
        /**
         *  "Pf.AppID" -> "APP_CODE"
         *  "Pf.SubAppID" -> "APP_CODE"
         *  "Pf.FakeAppID" -> "APP_CODE"
         *  "LogDir" -> "LOGGING.log_path"
         *  "Pf.PricefeedName" -> "APP_ID"
         *  "Pf.IsProd" -> ---
         */
        m_configMappingRules["LogDir"].emplace_back("LOGGING.log_path", Md2ConfigType::TypeString);
        m_configMappingRules["Pf.AppID"].emplace_back("APP_CODE", Md2ConfigType::TypeLong);
        m_configMappingRules["Pf.SubAppID"].emplace_back("APP_CODE", Md2ConfigType::TypeLong);
        m_configMappingRules["Pf.FakeAppID"].emplace_back("APP_CODE", Md2ConfigType::TypeLong);
        m_configMappingRules["Pf.PricefeedName"].emplace_back("APP_ID", Md2ConfigType::TypeLong);
        m_configMappingRules["Pf.IsProd"].emplace_back("false", Md2ConfigType::TypeConstant);

        /**
         * Pf.BfcEndpoint -> { CONNECTORS  { configs [ {exchange_mnemonic} ] } }
         */
        m_configMappingRules["Pf.BfcEndpoint"].emplace_back("CONNECTORS.configs", Md2ConfigType::TypeArray);
        m_configMappingRules["Pf.BfcEndpoint"].emplace_back("exchange_mnemonic", Md2ConfigType::TypeString);

        /**
         * Pf.RefDataHost -> { SYMBOLS  { secmaster_refdata_host } }
         * Pf.RefDataPort -> { SYMBOLS  { secmaster_refdata_port } }
         * Pf.RefDataPort -> { SYMBOLS  { secmaster_refdata_path } }
         * Pf.ExchDataHost -> { SYMBOLS  { secmaster_symbols_host } }
         * Pf.ExchDataPort -> { SYMBOLS  { secmaster_symbols_port } }
         * Pf.ExchDataPath -> { SYMBOLS  { secmaster_symbols_path } }
         * Pf.ExchSymbolPrefix -> { SYMBOLS  { exchange_symbol_prefix } }
         * Pf.DepthLimit -> { CONNECTORS { configs { orderbook_max_levels } } }
         */
        m_configMappingRules["Pf.RefDataHost"].emplace_back("SYMBOLS", Md2ConfigType::TypeObject);
        m_configMappingRules["Pf.RefDataHost"].emplace_back("secmaster_refdata_host", Md2ConfigType::TypeString);
        m_configMappingRules["Pf.RefDataPort"].emplace_back("SYMBOLS", Md2ConfigType::TypeObject);
        m_configMappingRules["Pf.RefDataPort"].emplace_back("secmaster_refdata_port", Md2ConfigType::TypeString);
        m_configMappingRules["Pf.RefDataPath"].emplace_back("SYMBOLS", Md2ConfigType::TypeObject);
        m_configMappingRules["Pf.RefDataPath"].emplace_back("secmaster_refdata_path", Md2ConfigType::TypeString);
        m_configMappingRules["Pf.ExchDataHost"].emplace_back("SYMBOLS", Md2ConfigType::TypeObject);
        m_configMappingRules["Pf.ExchDataHost"].emplace_back("secmaster_symbols_host", Md2ConfigType::TypeString);
        m_configMappingRules["Pf.ExchDataPort"].emplace_back("SYMBOLS", Md2ConfigType::TypeObject);
        m_configMappingRules["Pf.ExchDataPort"].emplace_back("secmaster_symbols_port", Md2ConfigType::TypeString);
        m_configMappingRules["Pf.ExchDataPath"].emplace_back("SYMBOLS", Md2ConfigType::TypeObject);
        m_configMappingRules["Pf.ExchDataPath"].emplace_back("secmaster_symbols_path", Md2ConfigType::TypeString);
        m_configMappingRules["Pf.ExchSymbolPrefix"].emplace_back("SYMBOLS", Md2ConfigType::TypeObject);
        m_configMappingRules["Pf.ExchSymbolPrefix"].emplace_back("exchange_symbol_prefix", Md2ConfigType::TypeString);
        m_configMappingRules["Pf.DepthLimit"].emplace_back("CONNECTORS.configs", Md2ConfigType::TypeArray);
        m_configMappingRules["Pf.DepthLimit"].emplace_back("orderbook_max_levels", Md2ConfigType::TypeInt);

        /**
         * <Exchange>.Mcast.Endpoint -> MD_SERVICE.[listen_ip : listen_port]
         */
        m_configMappingRules[".Mcast.Endpoint"].emplace_back("MD_SERVICE.listen_ip", Md2ConfigType::TypeString);
        m_configMappingRules[".Mcast.Endpoint"].emplace_back(":", Md2ConfigType::TypeConstant);
        m_configMappingRules[".Mcast.Endpoint"].emplace_back("MD_SERVICE.listen_port", Md2ConfigType::TypeString);

        /** 
         * <Exchange>.WebSocket.NumConcurrentSessions = 10  ->  ---
         * <Exchange>.WebSocket.Target -> { CONNECTORS  { configs [ {endpoint} ] } }
         * <Exchange>.WebSocket.ReconnectDelay ->  { CONNECTORS  { configs [ {retry_interval_secs} ] } }
         */
        m_configMappingRules[".WebSocket.NumConcurrentSessions"].emplace_back("10", Md2ConfigType::TypeConstant);
        m_configMappingRules[".WebSocket.Target"].emplace_back("CONNECTORS.configs", Md2ConfigType::TypeArray);
        m_configMappingRules[".WebSocket.Target"].emplace_back("endpoint", Md2ConfigType::TypeString);
        m_configMappingRules[".WebSocket.ReconnectDelay"].emplace_back("CONNECTORS.configs", Md2ConfigType::TypeArray);
        m_configMappingRules[".WebSocket.ReconnectDelay"].emplace_back("retry_interval_secs", Md2ConfigType::TypeInt);

        /**
         * <Exchange>.WebSocket.Endpoint -> { CONNECTORS  { configs [ {host : port} ] } }
         */
        m_configMappingRules[".WebSocket.Endpoint"].emplace_back("CONNECTORS.configs", Md2ConfigType::TypeArray);
        m_configMappingRules[".WebSocket.Endpoint"].emplace_back("host", Md2ConfigType::TypeString);
        m_configMappingRules[".WebSocket.Endpoint"].emplace_back(":", Md2ConfigType::TypeConstant);
        // m_configMappingRules[".WebSocket.Endpoint"].emplace_back("CONNECTORS.configs", Md2ConfigType::TypeArray);
        m_configMappingRules[".WebSocket.Endpoint"].emplace_back("port", Md2ConfigType::TypeInt);
    }

    std::string asStr(const std::string &aConfigKey, const std::string &aDefaultValue = "") {
        // remove exchange name if any
        std::string configKey = aConfigKey;
        size_t bItr = aConfigKey.find(".");
        size_t eItr = aConfigKey.find_last_of(".");
        if (bItr != std::string::npos && 
            eItr != std::string::npos &&
            bItr != eItr && bItr != 0) {
            configKey = aConfigKey.substr(bItr);
        }
        const auto crItr = m_configMappingRules.find(configKey);
        if (crItr == m_configMappingRules.end())
            return aDefaultValue;

        const rapidjson::Value *lastValuePtr = nullptr;   // composite value
        Md2ConfigType lastValueType = Md2ConfigType::TypeDefault;
        std::string result;
        for (const auto& rule : crItr->second) {
            switch (rule.configType()) {
            case Md2ConfigType::TypeArray:
                lastValuePtr = m_parser.getArray(rule.configName());
                lastValueType = rule.configType();
                break;
            case Md2ConfigType::TypeObject:
                lastValuePtr = m_parser.getObject(rule.configName());
                lastValueType = rule.configType();
                break;
            case Md2ConfigType::TypeString:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        result += m_parser.asStr(rule.configName(), &(*lastValuePtr)[0]);
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    result += m_parser.asStr(rule.configName(), lastValuePtr);
                break;
            case Md2ConfigType::TypeInt:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        result += std::to_string(m_parser.asInt(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    result += std::to_string(m_parser.asInt(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeLong:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        result += std::to_string(m_parser.asLong(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    result += std::to_string(m_parser.asLong(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeDouble:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        result += std::to_string(m_parser.asDouble(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    result += std::to_string(m_parser.asDouble(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeBool:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0) 
                        result += std::to_string(m_parser.asBool(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else 
                    result += std::to_string(m_parser.asBool(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeConstant:
                result += rule.configName();
                break;
            default:        // TypeDefault as well.
                return aDefaultValue;
            }
        }
        return result.empty() ? aDefaultValue : result;
    }

    int asInt(const std::string &aConfigKey, const int aDefaultValue = 0) {
        // remove exchange name if any
        std::string configKey = aConfigKey;
        size_t bItr = aConfigKey.find(".");
        size_t eItr = aConfigKey.find_last_of(".");
        if (bItr != std::string::npos && 
            eItr != std::string::npos &&
            bItr != eItr && bItr != 0) {
            configKey = aConfigKey.substr(bItr);
        }
        const auto crItr = m_configMappingRules.find(configKey);
        if (crItr == m_configMappingRules.end())
            return aDefaultValue;

        const rapidjson::Value *lastValuePtr = nullptr;   // composite value
        Md2ConfigType lastValueType = Md2ConfigType::TypeDefault;
        // std::string result;
        for (const auto& rule : crItr->second) {
            switch (rule.configType()) {
            case Md2ConfigType::TypeArray:
                lastValuePtr = m_parser.getArray(rule.configName());
                lastValueType = rule.configType();
                break;
            case Md2ConfigType::TypeObject:
                lastValuePtr = m_parser.getObject(rule.configName());
                lastValueType = rule.configType();
                break;
            case Md2ConfigType::TypeString:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return std::stoi(m_parser.asStr(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return std::stoi(m_parser.asStr(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeInt:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return m_parser.asInt(rule.configName(), &(*lastValuePtr)[0]);
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return m_parser.asInt(rule.configName(), lastValuePtr);
                break;
            case Md2ConfigType::TypeLong:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return static_cast<int>(m_parser.asLong(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return static_cast<int>(m_parser.asLong(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeDouble:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return static_cast<int>(m_parser.asDouble(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return static_cast<int>(m_parser.asDouble(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeConstant:
                // user can override hardcoded values.
                if (aDefaultValue == 0 && rule.configName().find_first_not_of("-0123456789") == std::string::npos)
                    return std::stoi(rule.configName());
                return aDefaultValue;
            default:        // TypeDefault as well.
                return aDefaultValue;
            }
        }
        return aDefaultValue;
    }

    long asLong(const std::string &aConfigKey, const long aDefaultValue = 0) {
        // remove exchange name if any
        std::string configKey = aConfigKey;
        size_t bItr = aConfigKey.find(".");
        size_t eItr = aConfigKey.find_last_of(".");
        if (bItr != std::string::npos && 
            eItr != std::string::npos &&
            bItr != eItr && bItr != 0) {
            configKey = aConfigKey.substr(bItr);
        }
        const auto crItr = m_configMappingRules.find(configKey);
        if (crItr == m_configMappingRules.end())
            return aDefaultValue;

        const rapidjson::Value *lastValuePtr = nullptr;   // composite value
        Md2ConfigType lastValueType = Md2ConfigType::TypeDefault;
        for (const auto& rule : crItr->second) {
            switch (rule.configType()) {
            case Md2ConfigType::TypeArray:
                lastValuePtr = m_parser.getArray(rule.configName());
                lastValueType = rule.configType();
                break;
            case Md2ConfigType::TypeObject:
                lastValuePtr = m_parser.getObject(rule.configName());
                lastValueType = rule.configType();
                break;
            case Md2ConfigType::TypeString:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return std::stol(m_parser.asStr(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return std::stol(m_parser.asStr(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeInt:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return m_parser.asInt(rule.configName(), &(*lastValuePtr)[0]);
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return m_parser.asInt(rule.configName(), lastValuePtr);
                break;
            case Md2ConfigType::TypeLong:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return m_parser.asLong(rule.configName(), &(*lastValuePtr)[0]);
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return m_parser.asLong(rule.configName(), lastValuePtr);
                break;
            case Md2ConfigType::TypeDouble:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return static_cast<int>(m_parser.asDouble(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return static_cast<int>(m_parser.asDouble(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeConstant:
                // user can override hardcoded values.
                if (aDefaultValue == 0 && rule.configName().find_first_not_of("-0123456789") == std::string::npos)
                    return std::stol(rule.configName());
                return aDefaultValue;
            default:        // TypeDefault as well.
                return aDefaultValue;
            }
        }
        return aDefaultValue;
    }

    bool asBool(const std::string &aConfigKey, const bool aDefaultValue = false) {
        // remove exchange name if any
        std::string configKey = aConfigKey;
        size_t bItr = aConfigKey.find(".");
        size_t eItr = aConfigKey.find_last_of(".");
        if (bItr != std::string::npos && 
            eItr != std::string::npos &&
            bItr != eItr && bItr != 0) {
            configKey = aConfigKey.substr(bItr);
        }
        const auto crItr = m_configMappingRules.find(configKey);
        if (crItr == m_configMappingRules.end())
            return aDefaultValue;

        const rapidjson::Value *lastValuePtr = nullptr;   // composite value
        Md2ConfigType lastValueType = Md2ConfigType::TypeDefault;
        for (const auto& rule : crItr->second) {
            switch (rule.configType()) {
            case Md2ConfigType::TypeArray:
                lastValuePtr = m_parser.getArray(rule.configName());
                lastValueType = rule.configType();
                break;
            case Md2ConfigType::TypeObject:
                lastValuePtr = m_parser.getObject(rule.configName());
                lastValueType = rule.configType();
                break;
            case Md2ConfigType::TypeString:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return m_parser.asStr(rule.configName(), &(*lastValuePtr)[0]) == "true";
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return m_parser.asStr(rule.configName(), lastValuePtr) == "true";
                break;
            case Md2ConfigType::TypeInt:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return static_cast<bool>(m_parser.asInt(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return static_cast<bool>(m_parser.asInt(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeLong:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return static_cast<bool>(m_parser.asLong(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return static_cast<bool>(m_parser.asLong(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeDouble:
                if (lastValuePtr != nullptr && lastValueType == Md2ConfigType::TypeArray) {
                    if (lastValuePtr->Size() > 0)
                        return static_cast<bool>(m_parser.asDouble(rule.configName(), &(*lastValuePtr)[0]));
                    else
                        return aDefaultValue;    // unexpected
                }
                else
                    return static_cast<bool>(m_parser.asDouble(rule.configName(), lastValuePtr));
                break;
            case Md2ConfigType::TypeConstant:
                // user can override hardcoded values.
                if (aDefaultValue != 0)
                    return aDefaultValue;
                return rule.configName() == "true";
            default:        // TypeDefault as well.
                return aDefaultValue;
            }
        }
        return aDefaultValue;
    }

    boost::posix_time::time_duration asTimeDuration(const std::string &aConfigKey, 
            const boost::posix_time::time_duration aDefaultValue = boost::posix_time::seconds(0)) {
        auto value = this->asInt(aConfigKey, 0);
        if (value != 0)
            return boost::posix_time::seconds(value);
        return aDefaultValue;
    }

private:
    Md2ConfigParser m_parser;
    std::map<std::string, std::vector<Md2ConfigRuleItem>> m_configMappingRules;

};

// Test Code.
int main() {
    const char* json = R"(
{
    "ROLE"   : "app",
    "APP_ID" : "MD2-HUOBI",
    "APP_CODE" : 12345,
    "LOGGING": {
        "log_level"             : "INFO",
        "log_path"              : "/home/ubuntu/Crypto/Logs/md2",
        "destinations"          : [
            {
                "log_type"      : "syslog",
                "log_facility"  : "user",
                "enabled"       : 1
            }
        ]
    },
    "SYMBOLS": {
        "symbol_mapping"    : "/home/ubuntu/Crypto/MD2/StaticData/symbology.csv",
        "secmaster_refdata_host" : "172.21.5.151",
        "secmaster_refdata_port" : "8001",
        "secmaster_refdata_path" : "/api/assetStores",
        "secmaster_symbols_host" : "172.21.2.170",
        "secmaster_symbols_port" : "9004",
        "secmaster_symbols_path" : "/api/v1/aliases/",
        "exchange_symbol_prefix" : "t"
    },
    "MD_SERVICE" : {
        "listen_ip"   : "0.0.0.0",
        "listen_port" : "9010",
        "admin_listen_ip"   : "0.0.0.0",
        "admin_listen_port" : "9110",
        "exchange_fees": "/home/ubuntu/Crypto/gdqt-config/StaticData/exchange-fees.json"
    },
    "CONNECTORS" : {
        "plugin_path" : "/home/ubuntu/Crypto/MD2/bin",
        "log_rotation_size_mb" : 100,
        "configs": [
        {
            "plugin_name": "MD2_HUOBI",
            "enabled" : 1,
            "exchange_mnemonic": "HUOBI",
            "host": "api-aws.huobi.pro",
            "port": 443,
            "endpoint": "/ws",
            "https_host": "api-aws.huobi.pro",
            "https_port": "443",
            "https_endpoint_prefix":"/v1",
            "enable_traffic_log": false,
            "market_depth": ".depth.step0",
            "orderbook_max_levels": 10,
            "log_rotation_size_mb" : 100,
            "traffic_log": "HUOBI.log.bz2",
            "retry_interval_secs" : 5,
            "symbol_check_interval_secs":120,
            "use_sni_hostname_for_ssl": true,
            "use_sni_hostname_for_ssl_ws": true
        }]
    }
}
)";

    std::string fileName("/tmp/md2_config_parser.cfg");
    std::ofstream ofs(fileName);
    ofs << json;
    ofs.close();

    // Test-1: Generic Parser
    Md2ConfigParser parser(fileName);
    std::cout << __func__ << ": ROLE: " << parser.asStr("ROLE") << std::endl;
    const auto * array = parser.getArray("CONNECTORS.configs");
    if (array != nullptr) {
        std::cout << __func__ << "received array of size: " << array->Size() << std::endl;
        std::cout << __func__ << ": exchange_mnemonic: " << parser.asStr("exchange_mnemonic", &(*array)[0]) << std::endl;
        std::cout << __func__ << ": host: " << parser.asStr("host", &(*array)[0]) << std::endl;
        std::cout << __func__ << ": port: " << parser.asInt("port", &(*array)[0]) << std::endl;
        std::cout << __func__ << ": use_sni_hostname_for_ssl: " << std::boolalpha << parser.asBool("use_sni_hostname_for_ssl", &(*array)[0]) << std::endl;
    }
    std::cout << "----------------------------------------\n";

    // Test-2: MD2 Mapper
    Md2ConfigMapper mapper(fileName);
    auto endpoint = mapper.asStr("Bitfinex.Mcast.Endpoint");
    std::cout << __func__ << ": Mcast Endpoint: " << endpoint << std::endl;
    endpoint = mapper.asStr("Bitfinex.WebSocket.Endpoint");
    std::cout << __func__ << ": WebSocket Endpoint: " << endpoint << std::endl;
    std::cout << __func__ << ": WebSocket ReconnectDelay: " << mapper.asInt("Bitfinex.WebSocket.ReconnectDelay") << std::endl;
    std::cout << __func__ << ": WebSocket NumConcurrentSessions: " << mapper.asInt("Bitfinex.WebSocket.NumConcurrentSessions") << std::endl;
    std::cout << __func__ << ": Pf.AppID: " << mapper.asLong("Pf.AppID") << std::endl;
    std::cout << __func__ << ": Pf.IsProd: " << mapper.asLong("Pf.IsProd") << std::endl;
    std::cout << __func__ << ": Pf.IsProd: " << mapper.asBool("Pf.IsProd") << std::endl;
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    std::cout << __func__ << ": WebSocket ReconnectDelay: " << to_simple_string(mapper.asTimeDuration("Bitfinex.WebSocket.ReconnectDelay")) << std::endl;
    std::cout << __func__ << ": Pf.DepthLimit: " << mapper.asInt("Pf.DepthLimit") << std::endl;

    std::cout << "----------------------------------------\n\n";

    // Load config values.
    const std::string refDataHost = mapper.asStr("Pf.RefDataHost");
    const std::string refDataPort = mapper.asStr("Pf.RefDataPort");
    const std::string refDataPath = mapper.asStr("Pf.RefDataPath");
    const std::string exchDataHost = mapper.asStr("Pf.ExchDataHost");
    const std::string exchDataPort = mapper.asStr("Pf.ExchDataPort");
    const std::string exchDataPath = mapper.asStr("Pf.ExchDataPath");
    
    const std::string bfcEndpointStr = mapper.asStr("Pf.BfcEndpoint");
    const std::string exchSymPrefix = mapper.asStr("Pf.ExchSymbolPrefix");

    ExchContracts exchContracts;
    // Load symbol mappings (exchange contracts) 
    loadExchContractsFromSecMaster(bfcEndpointStr,
            refDataHost,
            refDataPort,
            refDataPath,
            exchDataHost,
            exchDataPort,
            exchDataPath,
            exchContracts
        );
    std::cout << "Loaded " << exchContracts.size() << " contracts." << std::endl;

    for (int i=0; i < 10 && i < exchContracts.size(); ++i) {
        std::cout << i << ": local: " << exchContracts[i].bfcSym << ", exch: " << exchContracts[i].exchSym 
                  << ", final: " << (exchSymPrefix + exchContracts[i].exchSym) << std::endl;
    }
    return 0;
}
