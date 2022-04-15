// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>
#include <sstream>

using namespace rapidjson;

/**** L3Update Msg Format

{
    "type": "L3",
    "key":[<int64>, <int>],
    "symbol": <string>,
    "exchange": <string>,
    "id": <string>,
    "price": <double>,
    "qty": <double>,
    "side": <1|-1>,
    "isIntra": <bool>
}

****/

inline std::string FormatL3UpdateSstream(const std::string& type,
        const std::string& key,
        const std::string& symbol,
        const std::string& exchange,
        const std::string& id,
        const double price,
        const double qty,
        const int side,
        const bool isIntra) {
    std::ostringstream oss;
    // std::string res = (isIntra == true) ? "true" : "false";
        oss << "{"
        << R"("type":")" << type << R"(")"
        << R"(,"key":")"<< key << R"(")" 
        << R"(,"symbol":)" << symbol << R"(")"
        << R"(,"exchange":")" << exchange << R"(")"
        << R"(,"id":)" << id << R"(")"
        << R"(,"price":)" << price
        << R"(,"qty":)" << qty
        << R"(,"side":)" << side
        << R"(,"isIntra":)" << (isIntra == true? "true":"false")
        << "}";

    return oss.str();
}


/**
 * @brief Json L3Update message
 * 
 */
inline std::string FormatL3Update(const std::string& type,
        const std::string& key,
        const std::string& symbol,
        const std::string& exchange,
        const std::string& id,
        const double price,
        const double qty,
        const int side,
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

    aValue.SetString(id.c_str(), static_cast<SizeType>(id.size()), document.GetAllocator());
    document.AddMember("id", aValue, document.GetAllocator());

    aValue.SetDouble(price);
    document.AddMember("price", aValue, document.GetAllocator());

    aValue.SetDouble(qty);
    document.AddMember("qty", aValue, document.GetAllocator());

    aValue.SetInt(side);
    document.AddMember("side", aValue, document.GetAllocator());

    aValue.SetBool(isIntra);
    document.AddMember("isIntra", aValue, document.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(buffer);
    document.Accept(writer);
    // std::cout << buffer.GetString() << std::endl;
    return buffer.GetString();
}

/**
 * @brief Json L3Update decoding
 * s
 */
inline void ParseL3Update(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": key: " << document["key"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << __func__ << ": exchange: " << document["exchange"].GetString() << std::endl;
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": price: " << document["price"].GetDouble() << std::endl;
    std::cout << __func__ << ": qty: " << document["qty"].GetDouble() << std::endl;
    std::cout << __func__ << ": side: " << document["side"].GetInt() << std::endl;
    std::cout << __func__ << ": isIntra: " << document["isIntra"].GetBool() << std::endl;
}

// TODO:
// - need to make sure there is no memory leak(s)
// - need to make sure performance is equal or better than stringstream

int main() {
    std::string json;
    auto start = std::chrono::steady_clock::now();
    for(int i=0; i<1E6; i++)
    {
        json = FormatL3UpdateSstream("L2", "039874", "BTCUSDT", "BTFX", "99923", 23123.321, 1231.31, 1, 0);
    }
    auto end = std::chrono::steady_clock::now();

    std::cout << "Time Span Taken By StringStream: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl << std::endl;

    start = std::chrono::steady_clock::now();
    for(int i=0; i<1E6; i++)
    {
        json = FormatL3Update("L2", "039874", "BTCUSDT", "BTFX", "99923", 23123.321, 1231.31, 1, 0);
    }
    end = std::chrono::steady_clock::now();

    std::cout << "Time Span Taken By JsonFormat : " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl << std::endl;
    
    // std::cout << "Json: " << json << std::endl;
    ParseL3Update(json);
    return 0;
}