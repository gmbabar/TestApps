#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>
#include <sstream>

using namespace rapidjson;

namespace parser{
    namespace ExchangeCapabilities{
        /**** ExchangeCapabilities Msg Format
        {
            "type"            : "exchange_capabilities",
            "exchange"        : <string>,
            "hasL1"           : <bool>,
            "hasL2"           : <bool>,
            "hasL3"           : <bool>,
            "hasT"            : <bool>
        }
        ****/

        /**
         * @brief Json ExchangeCapabilities message
         * 
         */
        inline std::string FormatExchangeCapabilities(const std::string& type, 
            const std::string& exchange, 
            const bool hasL1,
            const bool hasL2,
            const bool hasL3,
            const bool hasT) {
            // DOM document
            std::ostringstream oss;
            oss << "{"
            << R"("type":")" << type << R"(")"
            << R"(, "exchange":")" << exchange << R"(")"
            << R"(, "hasL1":)" << (hasL1 ==  true ? "true":"false")
            << R"(, "hasL2":)" << (hasL2 ==  true ? "true":"false")
            << R"(, "hasL3":)" << (hasL3 ==  true ? "true":"false")
            << R"(, "hasT":)" << (hasT ==  true ? "true":"false")
            << "}";

            return oss.str();
        }

        /**
         * @brief Json ExchangeCapabilities parsing
         * 
         */
        inline void ParseExchangeCapabilities(const std::string& json) {
            Document document;
            document.Parse(json.c_str());

            // values.
            std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
            std::cout << __func__ << ": exchange: " << document["exchange"].GetString() << std::endl;
            std::cout << __func__ << ": hasL1: " << std::boolalpha << document["hasL1"].GetBool() << std::endl;
            std::cout << __func__ << ": hasL2: " << std::boolalpha << document["hasL2"].GetBool() << std::endl;
            std::cout << __func__ << ": hasL3: " << std::boolalpha << document["hasL3"].GetBool() << std::endl;
            std::cout << __func__ << ": hasT: " << std::boolalpha << document["hasT"].GetBool() << std::endl;
        }
    }
}