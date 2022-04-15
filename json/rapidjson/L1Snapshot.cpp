// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>
#include <sstream>

using namespace rapidjson;

/**** L1Snapshot Msg Format
{
    "type": "snapshot",
    "globalSeq" : <int64>,
    "inside" : {"bid":<double>, "bidqty":<double>, "exchange":<string>, "offer":<double>, "offerqty"
    :<double>}
        "isIntra":<bool>,
        "key":[<int64>,<int>],
        "live":<0|1>,
        "localSeq":<int64>,
        "microsSincEpoch":<int64>,
        "symbol":<string>
    }
}
****/



inline std::string FormatL1SnapshotSstream(const std::string& type, 
        const int globalSeq,
        const std::string& inside,
        const bool isIntra,
        const std::string& key,
        const int live,
        const int localSeq,
        const int microsSincEpoch,
        const std::string& symbol){
    std::ostringstream oss;
    oss << "{"
    << R"("type":")" << type << R"(")"
    << R"(,"globalSeq":)" << globalSeq
    << R"(,"inside":")"<< inside << R"(")" 
    << R"(,"isIntra":)" << (isIntra == true? "true":"false")
    << R"(,"key":")" << key << R"(")"
    << R"(,"live":)" << live
    << R"(,"localSeq":)" << localSeq
    << R"(,"microsSincEpoch":)" << microsSincEpoch
    << R"(,"symbol":")" << symbol << R"(")"
    << "}";

    return oss.str();
    }
                            

/**
 * @brief Json L1Snapshot message
 * 
 */
inline std::string FormatL1Snapshot(const std::string& type, 
                            const int globalSeq,
                            const std::string& inside,
                            const bool isIntra,
                            const std::string& key,
                            const int live,
                            const int localSeq,
                            const int microsSincEpoch,
                            const std::string& symbol
                            )
{
    // DOM document
    Document document; // UTF8<>
    document.SetObject();

    // Add values.
    Value aValue;
    aValue.SetString(type.c_str(), static_cast<SizeType>(type.size()), document.GetAllocator());
    document.AddMember("type", aValue, document.GetAllocator());

    aValue.SetInt(globalSeq);
    document.AddMember("globalSeq", aValue, document.GetAllocator());

    aValue.SetString(inside.c_str(), static_cast<SizeType>(inside.size()), document.GetAllocator());
    document.AddMember("inside", aValue, document.GetAllocator());

    aValue.SetBool(isIntra);
    document.AddMember("isIntra", aValue, document.GetAllocator());

    aValue.SetString(key.c_str(), static_cast<SizeType>(key.size()), document.GetAllocator());
    document.AddMember("key", aValue, document.GetAllocator());

    aValue.SetInt(live);
    document.AddMember("live", aValue, document.GetAllocator());

    aValue.SetInt(localSeq);
    document.AddMember("localSeq", aValue, document.GetAllocator());

    aValue.SetInt(microsSincEpoch);
    document.AddMember("microsSincEpoch", aValue, document.GetAllocator());

    aValue.SetString(symbol.c_str(), static_cast<SizeType>(symbol.size()), document.GetAllocator());
    document.AddMember("symbol", aValue, document.GetAllocator());



    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(buffer);
    document.Accept(writer);
    // std::cout << buffer.GetString() << std::endl;
    return buffer.GetString();
}

/**
 * @brief Json L1Snapshot decoding
 * s
 */
inline void ParseL1Snapshot(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": globalSeq: " << document["globalSeq"].GetInt() << std::endl;
    std::cout << __func__ << ": inside: " << document["inside"].GetString() << std::endl;
    std::cout << __func__ << ": isIntra: " << std::boolalpha << document["isIntra"].GetBool() << std::endl;
    std::cout << __func__ << ": key: " << document["key"].GetString() << std::endl;
    std::cout << __func__ << ": live: " << document["live"].GetInt() << std::endl;
    std::cout << __func__ << ": localSeq: " << document["localSeq"].GetInt() << std::endl;
    std::cout << __func__ << ": microsSincEpoch: " << document["microsSincEpoch"].GetInt() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
}

// TODO:
// - need to make sure there is no memory leak(s)
// - need to make sure performance is equal or better than stringstream

int main() {
    std::string json = FormatL1SnapshotSstream("snapshot", 2, "{\"\"}", true, "123", 1, 5, 213123, "BTFX:BTCUSDT");
    // std::string json = FormatL1Snapshot("snapshot", 2, "{\"\"}", true, "123", 1, 5, 213123, "BTFX:BTCUSDT");
    std::cout << "Json: " << json << std::endl;
    ParseL1Snapshot(json);
    return 0;
}