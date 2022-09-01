#include <iostream>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "bufrange.hpp"
#include <sstream>






using namespace rapidjson;
using namespace Parse::data;


/*
{"type": "l2_updates","symbol": "BTCUSD",
"changes": [["buy","9122.04","0.00121425"],[ "sell", "9122.07", "0.98942292"]...]]}
*/


inline void slowParseL2update(const std::string& json) {
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
}

/*
{"type": "trade","symbol": "BTCUSD","event_id": 3575573053,“timestamp”: 151231241,"price": "9004.21000000","quantity": "0.09110000","side": "buy"}
*/

inline void slowParseTrade(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    std::cout << "type: " << document["type"].GetString() << std::endl;
    std::cout << "symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << "event_id: " << document["event_id"].GetInt64() << std::endl;
    std::cout << "timestamp: " << document["timestamp"].GetInt64() << std::endl;
    std::cout << "price: " << document["price"].GetString() << std::endl;
    std::cout << "quantity: " << document["quantity"].GetString() << std::endl;
    std::cout << "side: " << document["side"].GetString() << std::endl;
   
}

/*
{"type": "l2_updates","symbol": "BTCUSD",
"changes": [["buy","9122.04","0.00121425"],...,[ "sell", "9122.07", "0.98942292"]...],
"trades": [{"type": "trade","symbol": "BTCUSD","event_id": 169841458,"timestamp": 1560976400428,
"price": "9122.04","quantity": "0.0073173","side": "sell"},...],
"auction_events":[{"type":"auction_result","symbol":"BTCUSD","time_ms":1656100800000,
"result":"failure","highest_bid_price":"21241.05","lowest_ask_price":"21246.50","collar_price":"21188.07"},
{"type":"auction_indicative","symbol":"BTCUSD","time_ms":1656100755000,"result":"failure",
"highest_bid_price":"21228.83","lowest_ask_price":"21234.33","collar_price":"21231.58"}]}

*/
inline void slowParseV2Marketdata(const std::string& json) {
    Document document;
    document.Parse(json.c_str());
    if(document["type"].IsString()){
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
    }
    if(document.HasMember("trades")) {
        auto var = document["trades"].GetArray();
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
    }
    if(document.HasMember("auction_events")) { 
        auto var = document["auction_events"].GetArray();
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
}



/*
-------l2-update
{"type":"l2_updates","symbol":"BTCUSD","changes":[["buy","21440.73","0"],["buy","21427.95","0.2"],["buy","21440.74","0.00402665"],
["sell","21468.93","0"],["sell","40861.60","0"],["sell","21468.09","0"],["sell","21465.09","1"],["sell","21459.01","0.425"],
["sell","21464.73","0"],["sell","21468.96","0.00311346"]]}
*/


inline void fastParseL2update(const std::string& json) {
    int firstPos = 0;
    int secondPos = 0;
    const char *aBuf = json.c_str();
    std::string price;
    std::string amount;
    std::string side;
    int start = json.find("\"type\"");
    start = json.find(":", start);
    int stop  = json.find("\",", start);
    ConstBufRange type(&aBuf[start]+2,&aBuf[stop]);
    std::cout << type.asStr() << std::endl;
    start = json.find("\"symbol\"");
    start = json.find(":", start);
    stop  = json.find("\",", start);
    ConstBufRange symbol(&aBuf[start]+2,&aBuf[stop]);
    std::cout << symbol.asStr() << std::endl;
    start = json.find("\"changes\"");
    start = json.find(":", start);
    stop  = json.find("]]", start);
    ConstBufRange changes(&aBuf[start]+1,&aBuf[stop]+2);
    std::string arr = changes.asStr();
    std::cout << "changes : " << arr << std::endl;
    while (true) {
        firstPos = arr.find("[", firstPos+1);
        secondPos = arr.find("]", secondPos+1);
        if(firstPos == std::string::npos || secondPos == std::string::npos || secondPos == arr.size()-1) {
            break;
        }
        ConstBufRange ar(&arr[firstPos],&arr[secondPos]+1);
        std::string innerArr = ar.asStr();
        std::cout << innerArr << std::endl; 
        int size = innerArr.size();
        int comma = innerArr.find("\",");
        int ncomma = innerArr.find("\",",comma+1);
        int lcol = innerArr.find("\"", ncomma+4);
        ConstBufRange side(&innerArr[2],&innerArr[comma]);
        ConstBufRange price(&innerArr[comma]+3,&innerArr[ncomma]);
        ConstBufRange amount(&innerArr[ncomma]+3,&innerArr[lcol]);
        std::cout << side.asStr() <<"|"<< price.asStr() << "| " << amount.asStr() << std::endl;
    }
}


inline void fastParseTrades(const std::string&json) {
    int start = 0;
    int stop = 0;
    int firstPos = 0;
    int secondPos = 0;
    const char *aBuf = json.c_str();
    start = json.find("\"trades\"");
    start = json.find(":", start);
    stop  = json.find("}],", start);
    ConstBufRange trades(&aBuf[start]+1,&aBuf[stop]+2);
    std::string arr = trades.asStr();   
    std::cout << "trades : " << arr << std::endl;
    while (true) {
        firstPos = arr.find("{", firstPos+1);
        secondPos = arr.find("}", secondPos+1);
        if(firstPos == std::string::npos || secondPos == std::string::npos || secondPos == arr.size()-1) {
            break;
        }
        ConstBufRange ar(&arr[firstPos],&arr[secondPos]+1);
        std::string innerArr = ar.asStr();
        int size = innerArr.size();
        std::cout << innerArr << std::endl;
        start = innerArr.find("\"type\"");
        start = innerArr.find(":\"trade\"", start);
        stop  = innerArr.find("\",", start);
        ConstBufRange type(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"symbol\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find("\",", start);
        ConstBufRange symbol(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"event_id\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find(",", start);
        ConstBufRange event_id(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"timestamp\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find(",", start);
        ConstBufRange timestamp(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"price\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find("\",", start);
        ConstBufRange price(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"quantity\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find("\",", start);
        ConstBufRange quantity(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"side\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find("\"}", start);
        ConstBufRange side(&innerArr[start]+2,&innerArr[stop]);

        std::cout << type.asStr() <<"| "<< symbol.asStr() << "| " << event_id.asStr() << "| " << timestamp.asStr() <<"| "<< price.asStr() << "| " << quantity.asStr() <<"| "<< side.asStr() << std::endl;
    }
}

inline void fastParseAuction(const std::string&json) {

    int start = 0;
    int stop = 0;
    int firstPos = 0;
    int secondPos = 0;
    const char *aBuf = json.c_str();
    start = json.find("\"auction_events\"");
    start = json.find(":", start);
    stop  = json.find("}]}", start);
    ConstBufRange auctions(&aBuf[start]+1,&aBuf[stop]+2);
    std::string arr = auctions.asStr();

    std::cout << "auction_events : " << arr << std::endl;
    while (true) {
        firstPos = arr.find("{", firstPos+1);
        secondPos = arr.find("}", secondPos+1);
        if(firstPos == std::string::npos || secondPos == std::string::npos || secondPos == arr.size()-1) {
            break;
        }
        ConstBufRange ar(&arr[firstPos],&arr[secondPos]+1);
        std::string innerArr = ar.asStr();
        std::cout << innerArr << std::endl;
        start = innerArr.find("\"type\"");
        start = innerArr.find(":\"auction", start);
        stop  = innerArr.find("\",", start);
        ConstBufRange type(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"symbol\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find("\",", start);
        ConstBufRange symbol(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"time_ms\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find(",", start);
        ConstBufRange timestamp(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"result\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find("\",", start);
        ConstBufRange result(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"highest_bid_price\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find("\",", start);
        ConstBufRange highest_bid_price(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"lowest_ask_price\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find("\",", start);
        ConstBufRange lowest_ask_price(&innerArr[start]+2,&innerArr[stop]);

        start = innerArr.find("\"collar_price\"");
        start = innerArr.find(":", start);
        stop  = innerArr.find("\"}", start);
        ConstBufRange collar_price(&innerArr[start]+2,&innerArr[stop]);
    
        std::cout << type.asStr() <<"| "<< symbol.asStr() << "| " << timestamp.asStr() <<"| "<< result.asStr() << "| " << highest_bid_price.asStr() <<"| "<< lowest_ask_price.asStr() <<"| "<< collar_price.asStr() << std::endl;
    }
}

/*
{"type": "trade","symbol": "BTCUSD","event_id": 3575573053,“timestamp”: 151231241,"price": "9004.21000000","quantity": "0.09110000","side": "buy"}
*/

inline void fastParseTrade(const std::string& json) {
    int start = 0;
    int stop = 0;
    int firstPos = 0;
    int secondPos = 0;
    const char *aBuf = json.c_str();

        start = json.find("\"type\"");
        start = json.find(":\"trade\"", start);
        stop  = json.find("\",", start);
        ConstBufRange type(&aBuf[start]+2,&aBuf[stop]);

        start = json.find("\"symbol\"");
        start = json.find(":", start);
        stop  = json.find("\",", start);
        ConstBufRange symbol(&aBuf[start]+2,&aBuf[stop]);

        start = json.find("\"event_id\"");
        start = json.find(":", start);
        stop  = json.find(",", start);
        ConstBufRange event_id(&json[start]+2,&json[stop]);

        start = json.find("\"timestamp\"");
        start = json.find(":", start);
        stop  = json.find(",", start);
        ConstBufRange timestamp(&json[start]+2,&json[stop]);

        start = json.find("\"price\"");
        start = json.find(":", start);
        stop  = json.find("\",", start);
        ConstBufRange price(&json[start]+2,&json[stop]);

        start = json.find("\"quantity\"");
        start = json.find(":", start);
        stop  = json.find("\",", start);
        ConstBufRange quantity(&json[start]+2,&json[stop]);

        start = json.find("\"side\"");
        start = json.find(":", start);
        stop  = json.find("\"}", start);
        ConstBufRange side(&json[start]+2,&json[stop]);

        std::cout << type.asStr() <<"| "<< symbol.asStr() << "| " << event_id.asStr() << "| " << timestamp.asStr() <<"| "<< price.asStr() << "| " << quantity.asStr() <<"| "<< side.asStr() << std::endl;

}

/*
{"type": "l2_updates","symbol": "BTCUSD",
"changes": [["buy","9122.04","0.00121425"],...,[ "sell", "9122.07", "0.98942292"]...],
"trades": [{"type": "trade","symbol": "BTCUSD","event_id": 169841458,"timestamp": 1560976400428,
"price": "9122.04","quantity": "0.0073173","side": "sell"},...],
"auction_events":[{"type":"auction_result","symbol":"BTCUSD","time_ms":1656100800000,
"result":"failure","highest_bid_price":"21241.05","lowest_ask_price":"21246.50","collar_price":"21188.07"},
{"type":"auction_indicative","symbol":"BTCUSD","time_ms":1656100755000,"result":"failure",
"highest_bid_price":"21228.83","lowest_ask_price":"21234.33","collar_price":"21231.58"}]}
*/

inline void fastParseV2Marketdata(const std::string& json) {

    fastParseL2update(json);
    if(json.find("trades")!=std::string::npos)
        fastParseTrades(json);
    if(json.find("auction_events")!=std::string::npos)
        fastParseAuction(json);
    
}