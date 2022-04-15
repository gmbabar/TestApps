// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>
#include <sstream>

using namespace rapidjson;

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
inline std::string FormatExchangeCapabilitiesSstream(const std::string& type, 
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
    Document document; // UTF8<>
    document.SetObject();

    // Add values.
    Value aValue;
    aValue.SetString(type.c_str(), static_cast<SizeType>(type.size()), document.GetAllocator());
    document.AddMember("type", aValue, document.GetAllocator());

    aValue.SetString(exchange.c_str(), static_cast<SizeType>(exchange.size()), document.GetAllocator());
    document.AddMember("exchange", aValue, document.GetAllocator());

    aValue.SetBool(hasL1);
    document.AddMember("hasL1", aValue, document.GetAllocator());

    aValue.SetBool(hasL2);
    document.AddMember("hasL2", aValue, document.GetAllocator());

    aValue.SetBool(hasL3);
    document.AddMember("hasL3", aValue, document.GetAllocator());

    aValue.SetBool(hasT);
    document.AddMember("hasT", aValue, document.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(buffer);
    document.Accept(writer);
    // std::cout << buffer.GetString() << std::endl;
    return buffer.GetString();
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

// TODO:
// - need to make sure there is no memory leak(s)
// - need to make sure performance is equal or better than stringstream

int main() {
    // std::string json = FormatExchangeCapabilities("exchange_capabilities", "BTFX", false, true, false, true);
    std::string json = FormatExchangeCapabilitiesSstream("exchange_capabilities", "BTFX", false, true, false, true);
    std::cout << "Json: " << json << std::endl;
    ParseExchangeCapabilities(json);
    return 0;
}