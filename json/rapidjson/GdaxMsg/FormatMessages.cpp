// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <string.h>
#include <cstring>
#include <chrono>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace rapidjson;

/*
-------------Subscribtion msg

{"type": "subscribe","product_ids": ["ETH-USD","ETH-EUR"],"channels": ["level2","heartbeat",{"name": "ticker","product_ids": ["ETH-BTC","ETH-USD"]}]}

-------------Unsubscribe msg

{"type": "unsubscribe","channels": ["heartbeat"]}

{"type": "unsubscribe","product_ids": ["ETH-USD","ETH-EUR"],"channels": ["ticker"]}
----Snapshot
{"type":"snapshot","product_id":"ETH-USD","asks":[["89","0009.989"],["34","0.9879"],["69","0.699696"]],"bids":[["89","0009.989"],["34","0.9879"],["69","0.699696"]]}
*/

/*
----RESPONSES----
----Subscription
{
    "type":"subscriptions",
    "channels":[{"name":"heartbeat","product_ids":["ETH-USD","ETH-EUR"]},{"name":"ticker","product_ids":["ETH-BTC","ETH-USD","ETH-EUR"]},{"name":"level2",product_ids":["ETH-USD","ETH-EUR"]}]
}

----L2Update
{
    "type":"l2update",
    "product_id":"ETHUSD",
    "changes":[["buy","1550.16","0.80679000"]],
    "time":"2022-07-19T12:09:18.390473Z"
}
----Ticker
{
    "type":"ticker",
    "sequence":32589047814,
    "product_id":"ETH-USD",
    "price":"1549.08",
    "open_24h":"1485.35",
    "volume_24h":"589548.02768130",
    "low_24h":"1450.45",
    "high_24h":"1631.32",
    "volume_30d":"8147557.57837948",
    "best_bid":"1548.81",
    "best_ask":"1549.08",
    "side":"buy",
    "time":"2022-07-19T12:09:26.348930Z",
    "trade_id":319738529,
    "last_size":"0.01803274"
}
----Heartbeat
{
    "type":"heartbeat",
    "last_trade_id":319738525,
    "product_id":"ETH-USD",
    "sequence":32589047800,
    "time":"2022-07-19T12:09:26.296459Z"
}
----Snapshot
{
    "type":"snapshot",
    "product_id":"ETH-USD"
    "asks":[["89","0009.989"],["34","0.9879"],["69","0.699696"]],
    "bids":[["89","0009.989"],["34","0.9879"],["69","0.699696"]],

}

*/

/*

------BACKUP_CODE_FOR_COMPLEX_JSON-------

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
----L2Update
{
    "type":"l2update",
    "product_id":"ETHUSD",
    "channels":[["buy","1550.16","0.80679000"]],
    "time":"2022-07-19T12:09:18.390473Z"
}
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

inline void parseL2updateMsg(const std::string& json) {

    Document document;
    document.Parse(json.c_str());
    std::cout << "type: " << document["type"].GetString() << std::endl;
    std::cout << "product_id: " << document["product_id"].GetString() << std::endl;
    auto var = document["channels"].GetArray();
    std::cout << "channels: [";
    for (int idx=0; idx<var.Size(); ++idx) {
        auto &arrVal = var[idx];
        if (arrVal.IsArray()) {
            auto jsonArr = arrVal.GetArray();
            for (int x=0; x<jsonArr.Size(); ++x) {
                std::cout << jsonArr[x].GetString() << " ";
            }
        }
    }
    std::cout << "]" << std::endl;
    std::cout << "time: " << document["time"].GetString() << std::endl;

}

inline void parseL2updateSs(const std::string& json) {
    size_t len = json.length();
	char ch;
    int idx = 0;
	int begin = idx;
	do {
		ch = json[idx];
		if (::isspace(ch)) 
            continue; 
		else if (ch == '{') {
			std::cout << json[idx] << std::endl;
			begin = idx+1;
	} else if (ch == '[') {
			std::cout << json.substr(begin, idx-begin);
			std::cout << json[idx];
			begin = idx+1;
	} else if (ch == ',' ) {
            // std::cout << begin << ":";
            if(begin >= 53 && begin <=70) {
                std::cout << json.substr(begin, idx-begin) << " ";
			    begin = idx+1;
        } else {
                std::cout << json.substr(begin, idx-begin) << std::endl;
			    begin = idx+1;
            }
    } else if (ch == ']') {
			std::cout  << json.substr(begin, idx-begin);
			std::cout << ch  ;
			begin = idx+1;
	} else if (ch == '}') {
			std::cout  << json.substr(begin, idx-begin) << std::endl;
			std::cout << ch  ;
			begin = idx+1;
            if(++idx >= len)
                std::cout << std::endl;
		} 
    }  while ( ++idx <= len ); 
}



/*
----Ticker
{
    "type":"ticker",
    "sequence":32589047814,
    "product_id":"ETH-USD",
    "price":"1549.08",
    "open_24h":"1485.35",
    "volume_24h":"589548.02768130",
    "low_24h":"1450.45",
    "high_24h":"1631.32",
    "volume_30d":"8147557.57837948",
    "best_bid":"1548.81",
    "best_ask":"1549.08",
    "side":"buy",
    "time":"2022-07-19T12:09:26.348930Z",
    "trade_id":319738529,
    "last_size":"0.01803274"
}
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

inline void parseTickerSs(const std::string &json) {
	size_t len = json.length();
	char ch;
    int idx = 0;
	int begin = idx;
	do {
		ch = json[idx];
		if (::isspace(ch)) { continue; }
		else if (ch == '[' || ch == '{') {
			std::cout << json[idx] << std::endl;
			begin = idx+1;
		} else if (ch == ',' ) {
			std::cout << json.substr(begin, idx-begin) << std::endl;
			begin = idx+1;
		} else if (ch == '}' || ch == ']') {
			std::cout  << json.substr(begin, idx-begin) << std::endl;
			std::cout << ch  << std::endl;
			begin = idx+1;
		} 
        }  while ( ++idx <= len ); 
}

/*
----Heartbeat
{
    "type":"heartbeat",
    "last_trade_id":319738525,
    "product_id":"ETH-USD",
    "sequence":32589047800,
    "time":"2022-07-19T12:09:26.296459Z"
}
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
{
    "type":"subscriptions",
    "channels":[{"name":"heartbeat","product_ids":["ETH-USD","ETH-EUR"]},
            {"name":"ticker","product_ids":["ETH-BTC","ETH-USD","ETH-EUR"]},
            {"name":"level2",product_ids":["ETH-USD","ETH-EUR"]}]
}

*/

inline void formatSubscriptionsMsg(std::ostringstream &oss,
    const std::string& type,
    const std::string& channels) {
        oss << "{"
            << R"("type":")" << type << R"(")"
            << R"(,"channels":)" << channels
            << "}" ;
}

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

/*
----Snapshot
{
    "type":"snapshot",
    "product_id":"ETH-USD",
    "asks":[["89","0009.989"],["34","0.9879"],["69","0.699696"]],
    "bids":[["89","0009.989"],["34","0.9879"],["69","0.699696"]]
}

*/
inline void formatSnapshotMsg(std::ostringstream &oss,
    const std::string& type,
    const std::string& product_id,
    const std::string& asks,
    const std::string& bids) {

    oss << "{"
        << R"("type":")" << type << R"(")"
        << R"(,"product_id":")" << product_id << R"(")"
        << R"(,"asks":)" << asks
        << R"(,"bids":)" << bids 
        << "}" ;
}

//---working on it----
inline void parseSnapshotSs(const std::string& json) {
    size_t len = json.length();
	char ch;
    int idx = 0;
	int begin = idx;
	do {
		ch = json[idx];
		if (::isspace(ch)) 
            continue; 
		else if (ch == '{') {
			std::cout << json[idx] << std::endl;
			begin = idx+1;
	} else if (ch == '[') {
			std::cout << json.substr(begin, idx-begin);
			std::cout << json[idx];
			begin = idx+1;
	} else if (ch == ',' ) {
            std::cout << begin << ":";
            if(begin >= 53 && begin <=70) {
                std::cout << json.substr(begin, idx-begin) << " ";
			    begin = idx+1;
        } else {
                std::cout << json.substr(begin, idx-begin) << std::endl;
			    begin = idx+1;
            }
    } else if (ch == ']') {
			std::cout  << json.substr(begin, idx-begin);
			std::cout << ch  ;
			begin = idx+1;
	} else if (ch == '}') {
			std::cout  << json.substr(begin, idx-begin) << std::endl;
			std::cout << ch  ;
			begin = idx+1;
            if(++idx >= len)
                std::cout << std::endl;
		} 
    }  while ( ++idx <= len ); 
}

int main() {
    std::ostringstream oss;
    std::cout << "----------------------- Snapshot Msg -----------------------" << std::endl;
    formatSnapshotMsg(oss, "snapshot", "ETH-USD",R"([["89","0009.989"],["34","0.9879"],["69","0.699696"]])",R"([["89","0009.989"],["34","0.9879"],["69","0.699696"]])");
    std::cout << "Json: " << oss.str() << std::endl;
    parseSnapshotSs(oss.str());
    oss.str("");
    std::cout << "----------------------- Subscriptions Msg -----------------------" << std::endl;
    formatSubscriptionsMsg(oss, "subscriptions", R"([{"name":"heartbeat","product_ids":["ETH-USD","ETH-EUR"]},{"name":"ticker","product_ids":["ETH-BTC","ETH-USD","ETH-EUR"]},{"name":"level2","product_ids":["ETH-USD","ETH-EUR"]}])");
    std::cout << "Json: " << oss.str() << std::endl;
    parseSubscriptionsMsg(oss.str());
    oss.str("");
    std::cout << "----------------------- L2update Msg -----------------------" << std::endl;
    formatL2updateMsg(oss,"l2update","ETHUSD",R"([["buy","1550.16","0.80679000"]])","2022-07-19T12:09:18.390473Z");
    std::cout << "Json: " << oss.str() << std::endl;
    parseL2updateSs(oss.str());
    parseL2updateMsg(oss.str());
    oss.str("");
    std::cout << "----------------------- Ticker Msg -----------------------" << std::endl;
    formatTickerMsg(oss, "ticker",32589047814, "ETH-USD","1549.08","1485.35","589548.02768130","1450.45","1631.32",
                "8147557.57837948","1548.81","1549.08","buy","2022-07-19T12:09:26.348930Z",319738529,"0.01803274");
    std::cout << "Json: " << oss.str() << std::endl;
    JsonParser(oss.str());
    parseTickerMsg(oss.str());
    parseTickerMsgSs(oss.str());
    oss.str("");
    std::cout << "----------------------- Heartbeat Msg -----------------------" << std::endl;
    formatHeartbeatMsg(oss,"heartbeat",319738525,"ETH-USD",32589047800,"2022-07-19T12:09:26.296459Z");
    std::cout << "Json: " << oss.str() << std::endl;
    parseHeartbeatMsg(oss.str());
    oss.str("");

    return 0;
}

