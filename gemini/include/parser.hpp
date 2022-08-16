#include <iostream>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include <sstream>




using namespace rapidjson;

/*
-----------Top-Of-Book
{"type":"update","eventId":136474377973,"socket_sequence":0,
"events":[{"type":"change","reason":"initial","price":"24018.28","delta":"0.32","remaining":"0.32","side":"bid"},
{"type":"change","reason":"initial","price":"24023.96","delta":"0.3","remaining":"0.3","side":"ask"}]}
*/

void ParseV1MarketData(const char* json) {

    Document doc;
    doc.Parse(json);
    std::cout << "type :" << doc["type"].GetString() << std::endl;
    std::cout << "eventId :" << doc["eventId"].GetInt64() << std::endl;
    std::cout << "socket_sequence :" << doc["socket_sequence"].GetInt() << std::endl;
    std::cout << "events : [" ;
    auto eventsArr = doc["events"].GetArray();
    for (size_t idx = 0; idx < eventsArr.Size(); idx++) {   
        auto &val = eventsArr[idx];
        if (val.IsObject()) {
            std::cout << "side :" << val["side"].GetString() << std::endl;
            std::cout << "price :" << val["price"].GetString() << std::endl; 
            std::cout << "delta :" << val["delta"].GetString() << std::endl;
            std::cout << "reason :" << val["reason"].GetString() << std::endl;
        }
    }
    std::cout << " ]" << std::endl;

}

/*
----------------TradeV1
{"type": "update","eventId": 5375547515,"timestamp": 1547760288,"timestampms": 1547760288001,"socket_sequence": 15,
"events": [{"type": "trade","tid": 5375547515,"price": "3632.54","amount": "0.1362819142","makerSide": "ask"}]}
*/

void ParseTrade(const char* json) {

    Document doc;
    doc.Parse(json);
    std::cout << "type :" << doc["type"].GetString() << std::endl;
    std::cout << "eventId :" << doc["eventId"].GetInt64() << std::endl;
    std::cout << "timestamp :" << doc["timestamp"].GetInt64() << std::endl;
    std::cout << "socket_sequence :" << doc["socket_sequence"].GetInt() << std::endl;
    std::cout << "events : [" ;
    auto eventsArr = doc["events"].GetArray();
    for (size_t idx = 0; idx < eventsArr.Size(); idx++) {   
        auto &val = eventsArr[idx];
        if (val.IsObject()) {
            std::cout << "type :" << val["type"].GetString() << std::endl;
            std::cout << "tid :" << val["tid"].GetInt64() << std::endl;
            std::cout << "price :" << val["price"].GetString() << std::endl;
            std::cout << "amount :" << val["amount"].GetString() << std::endl;
            std::cout << "makerSide :" << val["makerSide"].GetString() << std::endl;        
        }
    }
    std::cout << " ]" ;

}

/*
--------------Not in use currently

std::string GetType(const char* json) {
    Document doc;
    doc.Parse(json);
    auto eventsArr = doc["events"].GetArray();
    for (size_t idx = 0; idx < eventsArr.Size(); idx++) {   
        auto &val = eventsArr[idx];
        if (val.IsObject()) {
            if(val["type"].GetString() == "trade"){
            const char* type = "trade";
            return type;
            }
        }   
    }
    return "";
}
*/
    

/*
{"type": "l2_updates","symbol": "BTCUSD",
"changes": [["buy","9122.04","0.00121425"],...,[ "sell", "9122.07", "0.98942292"]...],
"trades": [{"type": "trade","symbol": "BTCUSD","event_id": 169841458,"timestamp": 1560976400428,"price": "9122.04","quantity": "0.0073173","side": "sell"},...],
"auction_events":[{"type":"auction_result","symbol":"BTCUSD","time_ms":1656100800000,"result":"failure","highest_bid_price":"21241.05","lowest_ask_price":"21246.50","collar_price":"21188.07"},
{"type":"auction_indicative","symbol":"BTCUSD","time_ms":1656100755000,"result":"failure","highest_bid_price":"21228.83","lowest_ask_price":"21234.33","collar_price":"21231.58"}]}

*/


inline void ParseV2MarketData(const std::string& json) {
    Document document;
    document.Parse(json.c_str());
    std::cout << "type: " << document["type"].GetString() << std::endl;
    std::cout << "symbol: " << document["symbol"].GetString() << std::endl;
    auto var = document["changes"].GetArray();
    std::cout << "changes : [" << std::endl;
    for (SizeType i = 0; i < var.Size(); i++) {
        auto arrVal = var[i].GetArray();
        std::cout << "[";
        for(SizeType i = 0; i < arrVal.Size(); i++)
        {
            auto val = arrVal[i].GetString();
            std::cout << val << ",";
        }
        std::cout << "\b]" << std::endl;
    }
    std::cout << "]" << std::endl;
    var = document["trades"].GetArray();
    std::cout << "trades : [" << std::endl;
    for (int idx=0; idx<var.Size(); ++idx) {
        auto &arrVal = var[idx];
        if (arrVal.IsObject()) {
            std::cout << "type: " << arrVal["type"].GetString() << std::endl;
            std::cout << "symbol: " << arrVal["symbol"].GetString() << std::endl;
            std::cout << "event_id: " << arrVal["event_id"].GetInt64() << std::endl;
            std::cout << "side: " << arrVal["side"].GetString() << std::endl;
            std::cout << "price: " << arrVal["price"].GetString() << std::endl;
            std::cout << "quantity: " << arrVal["quantity"].GetString() << std::endl;
        }
    }
    std::cout << " ]" << std::endl;
    var = document["auction_events"].GetArray();
    std::cout << "auction_events : [" << std::endl;
    for (int idx=0; idx<var.Size(); ++idx) {
        auto &arrVal = var[idx];
        if (arrVal.IsObject()) {
            std::cout << "type: " << arrVal["type"].GetString() << std::endl;
            std::cout << "symbol: " << arrVal["symbol"].GetString() << std::endl;
            std::cout << "time_ms: " << arrVal["time_ms"].GetInt64() << std::endl;
            std::cout << "result: " << arrVal["result"].GetString() << std::endl;
            std::cout << "highest_bid_price: " << arrVal["highest_bid_price"].GetString() << std::endl;
            std::cout << "lowest_ask_price: " << arrVal["lowest_ask_price"].GetString() << std::endl;
        }
    }
    std::cout << " ]" << std::endl;
}




   