// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>

using namespace rapidjson;

/**** Subscribe Msg Format
{
    "type"            : "subscribe",
    "exchange"        : <string>,
    "symbol"          : <string>,
    "level"           : "L1|L2|L3|S|T",
    "update_frequency": <int>,
    "time_window_secs": <int>
    "max_levels"      : <int>,
    "book_type"       : "T|M|R",
    "one_shot"        : <true|false>
}
****/

/**
 * @brief Json subscription message
 * 
 */
inline std::string FormatUnsubscribe(const std::string& type, 
                            const std::string& exchange, 
                            const std::string& symbol, 
                            const std::string& level) {
    // DOM document
    Document document; // UTF8<>
    document.SetObject();

    // Add values.
    Value aValue;
    aValue.SetString(type.c_str(), static_cast<SizeType>(type.size()), document.GetAllocator());
    document.AddMember("type", aValue, document.GetAllocator());

    aValue.SetString(exchange.c_str(), static_cast<SizeType>(exchange.size()), document.GetAllocator());
    document.AddMember("exchange", aValue, document.GetAllocator());

    aValue.SetString(symbol.c_str(), static_cast<SizeType>(symbol.size()), document.GetAllocator());
    document.AddMember("symbol", aValue, document.GetAllocator());

    aValue.SetString(level.c_str(), static_cast<SizeType>(level.size()), document.GetAllocator());
    document.AddMember("level", aValue, document.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(buffer);
    document.Accept(writer);
    // std::cout << buffer.GetString() << std::endl;
    return buffer.GetString();
}

/**
 * @brief Json subscription decoding
 * 
 */
inline void ParseUnsubscribe(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": exchange: " << document["exchange"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << __func__ << ": level: " << document["level"].GetString() << std::endl;
}

// TODO:
// - need to make sure there is no memory leak(s)
// - need to make sure performance is equal or better than stringstream

int main() {
    std::string json = FormatUnsubscribe("subscribe", "BTFX", "BTCUSDT", "L1");
    std::cout << "Json: " << json << std::endl;
    ParseUnsubscribe(json);
    return 0;
}