// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <chrono>

// using namespace rapidjson;

struct Md2ConfigParser {
    explicit Md2ConfigParser(const char *json) {     // should be testing only.
        m_doc.Parse(json);
        if (!m_doc.IsObject()) {
            std::cout << "Error: failed to parse json: \n" << json << std::endl;
            throw std::runtime_error("Invalid json");
        }
    }

    // explicit Md2ConfigParser(const std::string &fileName) {
    //     std::ifstream fileStream(fileName);
    //     std::stringstream buffer;
    //     if (!fileStream.is_open()) {
    //         buffer << "Couldn't open configuration file: " << fileName;
    //         std::cout << "Error: " << buffer.str() << std::endl;
    //         throw std::runtime_error(buffer.str());
    //     } 
    //     buffer << fileStream.rdbuf();
    //     m_jsonStr = buffer.str();
    //     m_doc.Parse(m_jsonStr.c_str());
    // }

    const rapidjson::Value* getObject(const std::string &aKey, const rapidjson::Value *aValuePtr = nullptr) {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = aValuePtr != nullptr ? aValuePtr : &m_doc;
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {
std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return nullptr;
                levelVal = &val[aConfigKey.c_str()];
            }
            const rapidjson::Value &val = (*levelVal)[configKeys.back().c_str()];
            if ((*levelVal)[configKeys.back().c_str()].IsObject())
                return &(*levelVal)[configKeys.back().c_str()];
                // return &(*levelVal)[configKeys.back().c_str()].GetObject();
            return nullptr;
        }

        const rapidjson::Value &jsonVal = (aValuePtr != nullptr ? (*aValuePtr) : m_doc);
        if ( jsonVal.HasMember(aKey.c_str()) && jsonVal[aKey.c_str()].IsObject() ) {
            return &jsonVal[aKey.c_str()];
        }
        return nullptr;
    }

    const rapidjson::Value* getArray(const std::string &aKey, const rapidjson::Value *aValuePtr = nullptr) {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = aValuePtr != nullptr ? aValuePtr : &m_doc;
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {
std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return nullptr;
                levelVal = &val[aConfigKey.c_str()];
            }
            const rapidjson::Value &val = (*levelVal)[configKeys.back().c_str()];
            if ((*levelVal)[configKeys.back().c_str()].IsArray())
                return &(*levelVal)[configKeys.back().c_str()];
            return nullptr;
        }

        const rapidjson::Value &jsonVal = (aValuePtr != nullptr ? (*aValuePtr) : m_doc);
        if ( jsonVal.HasMember(aKey.c_str()) && jsonVal[aKey.c_str()].IsArray() ) {
            return &jsonVal[aKey.c_str()];
        }
        return nullptr;
    }

    const std::string asStr(const std::string &aKey, const std::string &aDefaultValue = "", const rapidjson::Value *aValuePtr = nullptr) {
        if (aKey.find(".") != std::string::npos) {
            const rapidjson::Value *levelVal = &m_doc;
            std::vector<std::string> configKeys;
            boost::split(configKeys, aKey, boost::is_any_of("."));
            for (const auto &aConfigKey : configKeys) {

std::cout << __func__ << ": " << aConfigKey << std::endl;
                const rapidjson::Value &val = (*levelVal);
if (!val.IsObject()) {
std::cout << __func__ << ": Not an object: " << std::endl;
} 
if (!val.HasMember(aConfigKey.c_str())) {
std::cout << __func__ << ": key not found: " << aConfigKey << std::endl;
}

                if (!val.IsObject() || !val.HasMember(aConfigKey.c_str()))
                    return aDefaultValue;
                levelVal = &val[aConfigKey.c_str()];
            }
            const rapidjson::Value &val = (*levelVal)[configKeys.back().c_str()];
            if ((*levelVal)[configKeys.back().c_str()].IsString())
                return (*levelVal)[configKeys.back().c_str()].GetString();
            return aDefaultValue;
        }

        if ( m_doc.HasMember(aKey.c_str()) && m_doc[aKey.c_str()].IsString() ) {
            return m_doc[aKey.c_str()].GetString();
        }
        return aDefaultValue;
    }

    int asInt(const std::string &aKey, int aDefaultValue = 0) {
        if ( m_doc.HasMember(aKey.c_str()) && m_doc[aKey.c_str()].IsInt() ) {
            return m_doc[aKey.c_str()].GetInt();
        }
        return aDefaultValue;
    }

    long asLong(const std::string &aKey, long aDefaultValue = 0) {
        if ( m_doc.HasMember(aKey.c_str()) && m_doc[aKey.c_str()].IsInt64() ) {
            return m_doc[aKey.c_str()].GetInt64();
        }
        return aDefaultValue;
    }

    double asDouble(const std::string &aKey, double aDefaultValue = 0) {
        if ( m_doc.HasMember(aKey.c_str()) && m_doc[aKey.c_str()].IsDouble() ) {
            return m_doc[aKey.c_str()].GetDouble();
        }
        return aDefaultValue;
    }

    bool asBool(const std::string &aKey, bool aDefaultValue = false) {
        if ( m_doc.HasMember(aKey.c_str()) && m_doc[aKey.c_str()].IsBool() ) {
            return m_doc[aKey.c_str()].GetBool();
        }
        return aDefaultValue;
    }

    // bool asTimeDuration(const std::string &aKey, bool aDefaultValue = false) {
    //     if ( m_doc.HasMember(aKey.c_str()) && m_doc[aKey.c_str()].IsString() ) {
    //         return m_doc[aKey.c_str()].GetString();
    //     }
    //     return aDefaultValue;
    // }


private:
    // static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
    rapidjson::Document m_doc;
    std::string m_jonStr;
};

// Test.
int main() {
    const char* json = R"(
{
    "ROLE"   : "app",
    "APP_ID" : "MD2-HUOBI",
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
    "symbol_mapping"    : "/home/ubuntu/Crypto/MD2/StaticData/symbology.csv"
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

    Md2ConfigParser parser(json);
    std::cout << __func__ << ": ROLE: " << parser.asStr("ROLE") << std::endl;
    const auto * array = parser.getArray("CONNECTORS.configs");
    if (array != nullptr) {
        std::cout << __func__ << "received array of size: " << array->Size() << std::endl;
        std::cout << __func__ << ": exchange_mnemonic: " << parser.asStr("exchange_mnemonic", "", &(*array)[0]) << std::endl;
    }
    // std::cout << __func__ << ": ROLE: " << parser.asStr("CONNECTORS.configs.exchange_mnemonic") << std::endl;
    return 0;
}
