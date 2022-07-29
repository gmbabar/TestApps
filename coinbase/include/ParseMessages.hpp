// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <string.h>
#include <cstring>
#include <sstream>

using namespace rapidjson;

/*
-------------Subscribtion msg

{"type": "subscribe","product_ids": ["ETH-USD","ETH-EUR"],"channels": ["level2","heartbeat",{"name": "ticker"
,"product_ids": ["ETH-BTC","ETH-USD"]}]}

-------------Unsubscribe msg

{"type": "unsubscribe","channels": ["heartbeat"]}

{"type": "unsubscribe","product_ids": ["ETH-USD","ETH-EUR"],"channels": ["ticker"]}

*/


/*
----L2Update

{"type":"l2update","product_id":"ETHUSD","changes":[["buy","1550.16","0.80679000"]],"time":"2022-07-19T12:09:18.390473Z"}

*/

inline void formatL2updateMsg(std::ostringstream &oss,
    const std::string& type,
    const std::string& product_id,
    const std::string& changes,
    const std::string& time) {

    oss << "{"
        << R"("type":")" << type << R"(")"
        << R"(,"product_id":")" << product_id << R"(")"
        << R"(,"changes":)" << changes
        << R"(,"time":")" << time << R"(")"
        << "}";
}

inline void parseL2update(const std::string& json) {
    Document document;
    document.Parse(json.c_str());
    std::cout << "type: " << document["type"].GetString() << std::endl;
    std::cout << "product_id: " << document["product_id"].GetString() << std::endl;
    auto var = document["changes"].GetArray();
    std::cout << "changes: [";
    for (int idx=0; idx<var.Size(); ++idx) {
        auto &arrVal = var[idx];
        if (arrVal.IsArray()) {
            auto jsonArr = arrVal.GetArray();
            for (int x=0; x<jsonArr.Size(); ++x) {
                std::cout << jsonArr[x].GetString() << ",";
            }
        }
    }
    std::cout << "\b]" << std::endl;
    std::cout << "time: " << document["time"].GetString() << std::endl;
}

inline void parseL2updateSs(const std::string& json) {

    int start = json.find("\"changes\"");
    start = json.find(":", start);
    std::string arr = json.substr(start+1, json.find("]]")-start+1);
    start = json.find("\"product_id\"");
    start = json.find(":", start);
    int stop = json.find("\",", start);
    std::string symbol = json.substr(start+2, stop-start-2);
    std::cout << symbol << std::endl;

    int firstPos = 0;
    int secondPos = 0;
    int comma = 0;
    int comma2 = 0;
    std::string side;
    std::string price;
    std::string amount;
    while (true) {
        firstPos = arr.find("[", firstPos+1);
        secondPos = arr.find("]", secondPos+1);
        if(firstPos == std::string::npos || secondPos == std::string::npos || secondPos == arr.size()-1) {
            break;
        }
        std::string innerArr = arr.substr(firstPos, secondPos-firstPos+1);

        comma = innerArr.find("\",", comma+1);
        comma2 = innerArr.find("\",", comma+1);
        side = innerArr.substr(2, comma-2);
        price = innerArr.substr(comma+3, comma2-comma-3);
        amount = innerArr.substr(comma2+3, (innerArr.size() - innerArr.find("\"", comma2+2))-3);
        std::cout << side << ", " << price << ", " << amount << std::endl;
        comma = 0;
        comma2 = 0;
    }

}




/*
----Ticker

{"type":"ticker","sequence":32589047814,"product_id":"ETH-USD","price":"1549.08","open_24h":"1485.35","volume_24h":"589548.02768130","low_24h":"1450.45","high_24h":"1631.32","volume_30d":"8147557.57837948","best_bid":"1548.81","best_ask":"1549.08","side":"buy","time":"2022-07-19T12:09:26.348930Z","trade_id":319738529,"last_size":"0.01803274"}

*/

inline void formatTickerMsg(std::ostringstream &oss,
    const std::string& type,
    const long sequence,
    const std::string& product_id,
    const std::string& price,
    const std::string& open_24h,
    const std::string& volume_24h,
    const std::string& low_24h,
    const std::string& high_24h,
    const std::string& volume_30d,
    const std::string& best_bid,
    const std::string& best_ask,
    const std::string& side,
    const std::string& time,
    const long trade_id,
    const std::string& last_size) {
        oss <<"{"
            << R"("type":")" << type << R"(")"
            << R"(,"sequence": )" << sequence
            << R"(,"product_id":")" << product_id << R"(")"
            << R"(,"price":")" << price << R"(")"
            << R"(,"open_24h":")" << open_24h << R"(")"
            << R"(,"volume_24h":")" << volume_24h << R"(")"
            << R"(,"low_24h":")" << low_24h << R"(")"
            << R"(,"high_24h":")" << high_24h << R"(")"
            << R"(,"volume_30d":")" << volume_30d << R"(")"
            << R"(,"best_bid":")" << best_bid << R"(")"
            << R"(,"best_ask":")" << best_ask << R"(")"
            << R"(,"side":")" << side << R"(")"
            << R"(,"time":")" << time << R"(")"
            << R"(,"trade_id": )" << trade_id
            << R"(,"last_size":")" << last_size << R"(")"
            << "}" ;
}

//  For All Values in ticker

/*
inline void parseTickerMsg(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": sequence: " << document["sequence"].GetInt64() << std::endl;
    std::cout << __func__ << ": product_id: " << document["product_id"].GetString() << std::endl;
    std::cout << __func__ << ": price: " << document["price"].GetString() << std::endl;
    std::cout << __func__ << ": open_24h: " << document["open_24h"].GetString() << std::endl;
    std::cout << __func__ << ": volume_24h: " << document["volume_24h"].GetString() << std::endl;
    std::cout << __func__ << ": low_24h: " << document["low_24h"].GetString() << std::endl;
    std::cout << __func__ << ": high_24h: " << document["high_24h"].GetString() << std::endl;
    std::cout << __func__ << ": volume_30d: " << document["volume_30d"].GetString() << std::endl;
    std::cout << __func__ << ": best_bid: " << document["best_bid"].GetString() << std::endl;
    std::cout << __func__ << ": best_ask: " << document["best_ask"].GetString() << std::endl;
    std::cout << __func__ << ": side: " << document["side"].GetString() << std::endl;
    std::cout << __func__ << ": time: " << document["time"].GetString() << std::endl;
    std::cout << __func__ << ": trade_id: " << document["trade_id"].GetInt64() << std::endl;
    std::cout << __func__ << ": last_size: " << document["last_size"].GetString() << std::endl;
}
*/

// only give the required values

inline void parseTicker(const char *json) {
    Document document;
    document.Parse(json);
    std::cout << "type: " << document["type"].GetString() << std::endl;
    std::cout << "price: " << document["price"].GetString() << std::endl;
    std::cout << "last_size: " << document["last_size"].GetString() << std::endl;       
}

inline void parseTickerSs(const std::string &json) {
    std::string type;
    std::string px;
    std::string amt;
    
    int start = json.find("\"type\"");
    start = json.find(":", start);
    int stop = json.find("\",", start);
    type = json.substr(start+2, stop-start-2);
    start = json.find("\"price\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    px = json.substr(start+2, stop-start-2);
    start = json.find("\"last_size\"");
    start = json.find(":", start);
    stop = json.find("\"}", start);
    amt = json.substr(start+2, stop-start-2);
    std::cout << "type: " << type << std::endl;
    std::cout << "Price: " << px << std::endl;
    std::cout << "last_size: " << amt << std::endl;
}


/*
----Heartbeat

{"type":"heartbeat","last_trade_id":319738525,"product_id":"ETH-USD","sequence":32589047800,"time":
"2022-07-19T12:09:26.296459Z"}

*/
inline void formatHeartbeatMsg(std::ostringstream &oss,
    const std::string& type,
    const long last_trade_id,
    const std::string& product_id,
    const long sequence,
    const std::string time) {
        oss << "{"
            << R"("type":")" << type << R"(")"
            << R"(,"last_trade_id":)" << last_trade_id
            << R"(,"product_id":")" << product_id << R"(")"
            << R"(,"sequence":)" << sequence
            << R"(,"time":")" << time << R"(")"
            << "}";

}       


inline void parseHeartbeatMsg(const std::string& json) {
    Document document;
    document.Parse(json.c_str());
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": last_trade_id: " << document["last_trade_id"].GetInt64() << std::endl;
    std::cout << __func__ << ": product_id: " << document["product_id"].GetString() << std::endl;
    std::cout << __func__ << ": sequence: " << document["sequence"].GetInt64() << std::endl;
    std::cout << __func__ << ": time: " << document["time"].GetString() << std::endl;
}


/*

----Subscription

{"type":"subscriptions","channels":[{"name":"heartbeat","product_ids":["ETH-USD","ETH-EUR"]},{"name":
"ticker","product_ids":["ETH-BTC","ETH-USD","ETH-EUR"]},{"name":"level2",product_ids":["ETH-USD","ETH-EUR"]}]}

*/

/*
inline void parseSubscriptionsMsg(const std::string& json) {
    Document document;
    document.Parse(json.c_str());
    std::cout << "type: " << document["type"].GetString() << std::endl;
    auto var = document["channels"].GetArray();
    for (int idx=0; idx<var.Size(); ++idx) {
        auto &arrVal = var[idx];
        if (arrVal.IsString()) {
            std::cout << arrVal.GetString() << std::endl;
        } 
        else if (arrVal.IsObject()) {
            std::cout << "name: " << arrVal["name"].GetString() << std::endl;
            auto jsonArr = arrVal["product_ids"].GetArray();
            for (int x=0; x<jsonArr.Size(); ++x) {
                std::cout << jsonArr[x].GetString() << std::endl;
            }
        }
    }
}
*/

/*
----Subscription_Response

{"type":"subscriptions","channels":[{"name":"heartbeat","product_ids":["ETH-USD","ETH-EUR"]},
{"name":"ticker","product_ids":["ETH-BTC","ETH-USD","ETH-EUR"]},{"name":"level2",product_ids":
["ETH-USD","ETH-EUR"]}]}

*/

inline void parseSubscriptions(const std::string& json) {
    Document document;
    document.Parse(json.c_str());
    std::cout << "type: " << document["type"].GetString() << std::endl;
    auto var = document["channels"].GetArray();
    std::cout << "channels: [";
    for (int idx=0; idx<var.Size(); ++idx) {
        auto &arrVal = var[idx];
        if (arrVal.IsString()) 
            std::cout << arrVal.GetString() << ","; 
        else if (arrVal.IsObject()) {
            std::cout << "  name: " << arrVal["name"].GetString() << std::endl;
            auto jsonArr = arrVal["product_ids"].GetArray();
            std::cout << "  product_ids:[ ";
            for (int x=0; x<jsonArr.Size(); ++x) {
                std::cout << jsonArr[x].GetString() << ",";
            }
            std::cout << "\b]" << std::endl;
        }
    }
    std::cout << "]" << std::endl;
}

/*
----Snapshot
{"type":"snapshot","product_id":"ETH-USD","asks":[["89","0009.989"],["34","0.9879"],["69","0.699696"]],bids":[["89","0009.989"],["34","0.9879"],["69","0.699696"]]}

*/

inline void parseSnapshot(const char *json) {
    Document document;
    document.Parse(json);
    std::cout << "Type : " << document["type"].GetString() << std::endl;
    std::cout << "product Id : " << document["product_id"].GetString() << std::endl;
    //Gets The Value Of Asks
    auto asks = document["asks"].GetArray();
    std::cout << "Asks : [" << std::endl;
    for (SizeType i = 0; i < asks.Size(); i++) {
        auto arrVal = asks[i].GetArray();
        std::cout << "Asks : [";
        for(SizeType i = 0; i < arrVal.Size(); i++)
        {
            auto val = arrVal[i].GetString();
            std::cout << val << ",";
        }
        std::cout << "\b]" << std::endl;
    }
    std::cout << "]" << std::endl;
    
    auto bids = document["bids"].GetArray();
    std::cout << "bids : [" << std::endl;
    for (SizeType i = 0; i < bids.Size(); i++) {
        auto arrVal = bids[i].GetArray();
        std::cout << "Bids : [";
        for(SizeType i = 0; i < arrVal.Size(); i++) {
            auto val = arrVal[i].GetString();
            std::cout << val << ",";
        }
        std::cout << "\b]" << std::endl;
    }
    std::cout << "]" << std::endl;
}


inline void parseSnapshotSs(const std::string& json) { 
    int firstPos = 0;
    int secondPos = 0;
    std::string price;
    std::string amount;
    int start = json.find("\"type\"");
    start = json.find(":", start);
    int stop  = json.find("\",", start);
    std::cout << json.substr(start+2, stop-start-2) << std::endl;
    start = json.find("\"product_id\"");
    start = json.find(":", start);
    stop  = json.find("\",", start);
    std::cout << json.substr(start+2, stop-start-2) << std::endl;
    start = json.find("\"asks\"");
    start = json.find(":", start);
    stop  = json.find("]],", start);
    std::string arr = json.substr(start+1, stop-start+1);
    std::cout << "ASKS : " << arr << std::endl;
    while (true) {
        /* code */
        firstPos = arr.find("[", firstPos+1);
        secondPos = arr.find("]", secondPos+1);
        if(firstPos == std::string::npos || secondPos == std::string::npos || secondPos == arr.size()-1) {
            break;
        }
        std::string innerArr = arr.substr(firstPos, secondPos-firstPos+1);
        int comma = innerArr.find("\",");
        price = innerArr.substr(2, comma-2); 
        amount = innerArr.substr(comma+3, (innerArr.size() - innerArr.find("\"", comma+2))-3);
        std::cout << price << ", " << amount << std::endl;
    }
    firstPos = 0;
    secondPos = 0;
    start = json.find("\"bids\"");
    start = json.find(":", start);
    stop  = json.find("]]", start);
    arr = json.substr(start+1, stop-start+1);
    std::cout << "BIDS : " << arr << std::endl;
    while (true) {
        /* code */
        firstPos = arr.find("[", firstPos+1);
        secondPos = arr.find("]", secondPos+1);
        if(firstPos == std::string::npos || secondPos == std::string::npos || secondPos == arr.size()-1) {
            break;
        }
        std::string innerArr = arr.substr(firstPos, secondPos-firstPos+1);
        int comma = innerArr.find("\",");
        price = innerArr.substr(2, comma-2); 
        amount = innerArr.substr(comma+3, (innerArr.size() - innerArr.find("\"", comma+2))-3);
        std::cout << price << ", " << amount << std::endl;
    }
}
