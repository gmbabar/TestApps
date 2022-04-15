// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>
#include <sstream>

using namespace rapidjson;

/**** TradeUpdate Msg Format

{
    "type": "trade",
    "key":[<int64>, <int>],
    "symbol": <string>,
    "exchange": <string>,
    "price": <double>,
    "qty": <double>,
    "isIntra": <bool>
}
****/
inline std::string FormatTradeUpdateSstream(const std::string& type,
        const std::string& key,
        const std::string& symbol,
        const std::string& exchange,
        const double price,
        const double qty,
        const bool isIntra) {
            std::ostringstream oss;
            oss << "{"
            <<R"("type":")"<< type <<R"(")"
            <<R"(,"key":")"<< key <<R"(")"
            <<R"(,"symbol":")"<< symbol <<R"(")"
            <<R"(,"exchange":")"<< exchange <<R"(")"
            <<R"(,"price":)"<< price 
            <<R"(,"qty":)"<< qty
            <<R"(,"isIntra":)"<< (isIntra == true? "true":"false")
            <<"}";
        return oss.str();    
    }

/**
 * @brief Json TradeUpdate message
 * 
 */
inline std::string FormatTradeUpdate(const std::string& type,
                            const std::string& key,
                            const std::string& symbol,
                            const std::string& exchange,
                            const double price,
                            const double qty,
                            const bool isIntra) {
    // DOM document
    Document document; // UTF8<>
    document.SetObject();

    // Add values.
    Value aValue;
    aValue.SetString(type.c_str(), static_cast<SizeType>(type.size()), document.GetAllocator());
    document.AddMember("type", aValue, document.GetAllocator());

    aValue.SetString(key.c_str(), static_cast<SizeType>(key.size()), document.GetAllocator());
    document.AddMember("key", aValue, document.GetAllocator());

    aValue.SetString(symbol.c_str(), static_cast<SizeType>(symbol.size()), document.GetAllocator());
    document.AddMember("symbol", aValue, document.GetAllocator());

    aValue.SetString(exchange.c_str(), static_cast<SizeType>(exchange.size()), document.GetAllocator());
    document.AddMember("exchange", aValue, document.GetAllocator());

    aValue.SetDouble(price);
    document.AddMember("price", aValue, document.GetAllocator());

    aValue.SetDouble(qty);
    document.AddMember("qty", aValue, document.GetAllocator());

    aValue.SetBool(isIntra);
    document.AddMember("isIntra", aValue, document.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(buffer);
    document.Accept(writer);
    // std::cout << buffer.GetString() << std::endl;
    return buffer.GetString();
}

/**
 * @brief Json TradeUpdate parsing
 * s
 */
inline void ParseTradeUpdate(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": key: " << document["key"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << __func__ << ": exchange: " << document["exchange"].GetString() << std::endl;
    std::cout << __func__ << ": price: " << document["price"].GetDouble() << std::endl;
    std::cout << __func__ << ": qty: " << document["qty"].GetDouble() << std::endl;
    std::cout << __func__ << ": isIntra: " << document["isIntra"].GetBool() << std::endl;
}

// TODO:
// - need to make sure there is no memory leak(s)
// - need to make sure performance is equal or better than stringstream

int main() {
    // std::string json = FormatTradeUpdate("L2", "039874", "BTCUSDT", "BTFX", 23123.321, 1231.31, 0);
    std::string json = FormatTradeUpdateSstream("L2", "039874", "BTCUSDT", "BTFX", 23123.321, 1231.31, 0);
    std::cout << "Json: " << json << std::endl;
    ParseTradeUpdate(json);
    return 0;
}