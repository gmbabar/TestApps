// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>
#include <sstream>

using namespace rapidjson;

/**** SymbolStatus Msg Format
{
  "type"     : "security_list_update",
  "action"   : "add",
  "symbol"   : <exchange>:<string>
}
****/
inline std::string FormatSymbolStatusSstream(const std::string& type, 
        const std::string& action, 
        const std::string& symbol) {
            std::stringstream oss;
            oss << "{"
            << R"("type":")" << type << R"(")"
            << R"(,"action":")" << action << R"(")"
            << R"(,"symbol":")" << symbol << R"(")"
            << "}";

        return oss.str();
    }

/**
 * @brief Json SymbolStatus message
 * 
 */
inline std::string FormatSymbolStatus(const std::string& type, 
                            const std::string& action, 
                            const std::string& symbol) {
    // DOM document
    Document document; // UTF8<>
    document.SetObject();

    // Add values.
    Value aValue;
    aValue.SetString(type.c_str(), static_cast<SizeType>(type.size()), document.GetAllocator());
    document.AddMember("type", aValue, document.GetAllocator());

    aValue.SetString(action.c_str(), static_cast<SizeType>(action.size()), document.GetAllocator());
    document.AddMember("action", aValue, document.GetAllocator());

    aValue.SetString(symbol.c_str(), static_cast<SizeType>(symbol.size()), document.GetAllocator());
    document.AddMember("symbol", aValue, document.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(buffer);
    document.Accept(writer);
    // std::cout << buffer.GetString() << std::endl;
    return buffer.GetString();
}

/**
 * @brief Json SymbolStatus parsing
 * 
 */
inline void ParseSymbolStatus(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": action: " << document["action"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
}

// TODO:
// - need to make sure there is no memory leak(s)
// - need to make sure performance is equal or better than stringstream

int main() {
    // std::string json = FormatSymbolStatus("security_list_update", "add", "BTFX:BTCUSDT");
    std::string json = FormatSymbolStatusSstream("security_list_update", "add", "BTFX:BTCUSDT");
    std::cout << "Json: " << json << std::endl;
    ParseSymbolStatus(json);
    return 0;
}