#include <iostream>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include <sstream>

using namespace rapidjson;


/*
-----------Top-Of-Book

{"ch":"orderbook/top/1000ms","data":{"BTCUSDT":{"t":1661349769825,"a":"21449.01","A":"0.32500","b":"21446.73","B":"0.32500"}}} 

*/

/*
----------Trade-Update
{"ch":"trades","update":{"BTCUSDT":[{"t":1661352113934,"i":1872478761,"p":"21460.55","q":"0.00105","s":"buy"},{"t":1661352113934,"i":1872478762,"p":"21460.56","q":"0.07000","s":"buy"}]}}
*/

inline void parseTrade(const std::string& json) {
    Document document;
    document.Parse(json.c_str());
    std::cout << "channel: " << document["trades"].GetString() << std::endl;
    auto obj = document["update"].IsObject() ? document["update"].GetObject() : document["snapshot"].GetObject();
    std::cout << "symbol: " << obj["symbol"].GetString() << std::endl;
    const char* symbol = obj["symbol"].GetString();
    auto var = document[symbol].GetArray();
    document["update"].IsObject() ? std::cout << "update : [" << std::endl : std::cout << "snapshot : [" << std::endl;
    // std::cout << "update : [" << std::endl;
    for (SizeType i = 0; i < var.Size(); i++) {
        auto arrVal = var[i].GetObject();
        std::cout << "time_ms: " << arrVal["t"].GetInt64() << std::endl;
        std::cout << "Trade identifier: " << arrVal["i"].GetInt64() << std::endl;
        std::cout << "price: " << arrVal["p"].GetString() << std::endl;
        std::cout << "quantity: " << arrVal["q"].GetString() << std::endl;
        std::cout << "side: " << arrVal["s"].GetString() << std::endl;
    }
    std::cout << "]" << std::endl;

}

/*
----------Trade-Snapshot
*/
// inline void parseTradeSnapshot(const std::string& json) {
//     Document document;
//     document.Parse(json.c_str());
//     std::cout << "channel: " << document["trades"].GetString() << std::endl;
//     auto obj = document["update"].GetObject();
//     std::cout << "symbol: " << obj["symbol"].GetString() << std::endl;
//     const char* symbol = obj["symbol"].GetString();
//     auto var = document[symbol].GetArray();
//     std::cout << "update : [" << std::endl;
//     for (SizeType i = 0; i < var.Size(); i++) {
//         auto arrVal = var[i].GetObject();
//         std::cout << "time_ms: " << arrVal["t"].GetInt64() << std::endl;
//         std::cout << "Trade identifier: " << arrVal["i"].GetInt64() << std::endl;
//         std::cout << "price: " << arrVal["p"].GetString() << std::endl;
//         std::cout << "quantity: " << arrVal["q"].GetString() << std::endl;
//         std::cout << "side: " << arrVal["s"].GetString() << std::endl;
//     }
//     std::cout << "]" << std::endl;

// }