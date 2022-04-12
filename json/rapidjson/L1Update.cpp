// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>

using namespace rapidjson;

/**** L1Update Msg Format
{
    "type": "inside",
    "key":[<int64>, <int>],
    "bid":<double>,
    "bidqty":<double>,
    "offer":<double>,
    "offerqty":<double>,
    "exchange":<string>,
    "isIntra": <bool>
}
****/

/**
 * @brief Json L1Update message
 * 
 */
inline std::string FormatL1Update(const std::string& type, 
                            const std::string& key,
                            const double bid,
                            const double bidqty,
                            const double offer,
                            const double offerqty,
                            const std::string& exchange,
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

    aValue.SetDouble(bid);
    document.AddMember("bid", aValue, document.GetAllocator());

    aValue.SetDouble(bidqty);
    document.AddMember("bidqty", aValue, document.GetAllocator());

    aValue.SetDouble(offer);
    document.AddMember("offer", aValue, document.GetAllocator());

    aValue.SetDouble(offerqty);
    document.AddMember("offerqty", aValue, document.GetAllocator());

    aValue.SetString(exchange.c_str(), static_cast<SizeType>(exchange.size()), document.GetAllocator());
    document.AddMember("exchange", aValue, document.GetAllocator());

    aValue.SetBool(isIntra);
    document.AddMember("isIntra", aValue, document.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(buffer);
    document.Accept(writer);
    // std::cout << buffer.GetString() << std::endl;
    return buffer.GetString();
}

/**
 * @brief Json SymbolStatus decoding
 * s
 */
inline void ParseL1Update(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": key: " << document["key"].GetString() << std::endl;
    std::cout << __func__ << ": bid: " << document["bid"].GetDouble() << std::endl;
    std::cout << __func__ << ": bidqty: " << document["bidqty"].GetDouble() << std::endl;
    std::cout << __func__ << ": offer: " << document["offer"].GetDouble() << std::endl;
    std::cout << __func__ << ": offerqty: " << document["offerqty"].GetDouble() << std::endl;
    std::cout << __func__ << ": exchange: " << document["exchange"].GetString() << std::endl;
    std::cout << __func__ << ": isIntra: " << document["isIntra"].GetBool() << std::endl;
}

// TODO:
// - need to make sure there is no memory leak(s)
// - need to make sure performance is equal or better than stringstream

int main() {
    std::string json = FormatL1Update("inside", "2554324", 25.254, 13.354, 76.312, 28.251, "BTFX", true);
    std::cout << "Json: " << json << std::endl;
    ParseL1Update(json);
    return 0;
}