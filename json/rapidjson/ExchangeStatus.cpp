// ExchangeStatusON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>

using namespace rapidjson;

/**** ExchangeStatus Msg Format
{
  "type"     : "exchange_status",
  "exchange" : <string>,
  "connected": <0|1>
}
****/

/**
 * @brief Json ExchangeStatus message
 * 
 */
inline std::string FormatExchangeStatus(const std::string& type, 
                            const std::string& exchange, 
                            int connected) {
    // DOM document
    Document document; // UTF8<>
    document.SetObject();

    // Add values.
    Value aValue;
    aValue.SetString(type.c_str(), static_cast<SizeType>(type.size()), document.GetAllocator());
    document.AddMember("type", aValue, document.GetAllocator());

    aValue.SetString(exchange.c_str(), static_cast<SizeType>(exchange.size()), document.GetAllocator());
    document.AddMember("exchange", aValue, document.GetAllocator());

    aValue.SetInt(connected);
    document.AddMember("connected", aValue, document.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(buffer);
    document.Accept(writer);
    // std::cout << buffer.GetString() << std::endl;
    return buffer.GetString();
}

/**
 * @brief Json ExchangeStatus decoding
 * s
 */
inline void ParseExchangeStatus(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": exchange: " << document["exchange"].GetString() << std::endl;
    std::cout << __func__ << ": connected: " << document["connected"].GetInt() << std::endl;
}

// TODO:
// - need to make sure there is no memory leak(s)
// - need to make sure performance is equal or better than stringstream

int main() {
    std::string json = FormatExchangeStatus("exchange_status", "BTFX", 0);
    std::cout << "Json: " << json << std::endl;
    ParseExchangeStatus(json);
    return 0;
}