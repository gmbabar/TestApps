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
using namespace std;

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
inline void strpnt(const char * str, int start, int stop) {
    for (int i = start; i <= stop; i++) {
        std::cout << str[i];
    }
    std::cout << std::endl;
}

/**
 * 
 * 
        std::string type;
        tokenPos = json.find("\"type\"");
        if(tokenPos != std::string::npos) {
            colonPos = json.find(":", tokenPos);
            commaPos = json.find("\",", colonPos);
            type = json.substr(colonPos+2, commaPos-colonPos-2);
            std::cout << type << std::endl;
        }
        else {
            std::cout << "Type Not Available" << std::endl;
        }
 * 
 */


inline void arrayParse(std::string &arr) {
    int firstPos = 0;
    int secondPos = 0;
    std::string price;
    std::string amount;
    while (true) {
        /* code */
        firstPos = arr.find("[", firstPos+1);
        secondPos = arr.find("]", secondPos+1);
        if(firstPos == string::npos || secondPos == string::npos || secondPos == arr.size()-1) {
            break;
        }
        std::string innerArr = arr.substr(firstPos, secondPos-firstPos+1);
        int comma = innerArr.find("\",");
        price = innerArr.substr(2, comma-2); 
        amount = innerArr.substr(comma+3, (innerArr.size() - innerArr.find("\"", comma+2))-3);
        std::cout << price << ", " << amount << std::endl;
    }
}

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
	    } 
        else if (ch == '[') {
	    	std::cout << json.substr(begin, idx-begin);
	    	std::cout << json[idx];
	    	begin = idx+1;
	    } 
        else if (ch == ',' ) {
            if(begin >= 53 && begin <= 70) {
                std::cout << json.substr(begin, idx-begin) << " ";
	    	    begin = idx+1;
            } 
            else {
                std::cout << json.substr(begin, idx-begin) << std::endl;
	    	    begin = idx+1;
            }
        } 
        else if (ch == ']') {
	    	std::cout  << json.substr(begin, idx-begin);
	    	std::cout << ch;
	    	begin = idx+1;
	    } 
        else if (ch == '}') {
	    	std::cout  << json.substr(begin, idx-begin) << std::endl;
	    	std::cout << ch;
	    	begin = idx+1;
            if(++idx >= len)
                std::cout << std::endl;
	    } 
    }  while ( ++idx <= len ); 
}

inline void parseL2updateSsOptm(const std::string& json) {
    
    int start = json.find("{type");
    int stop  = json.find("\",");
    strpnt(json.c_str(), start, stop);
    start = json.find("\"product_id");
    stop  = json.find("\",", start);
    strpnt(json.c_str(), start, stop);
    start = json.find("\"changes");
    start = json.find(":", start);
    stop  = json.find("]],", start);
    // strpnt(json.c_str(), start, stop);
    std::string arr = json.substr(start+1, stop-start+1);
    start = json.find("\"time");
    stop  = json.find("}", start);
    strpnt(json.c_str(), start, stop);
}

/*
----Ticker

{"type":"ticker","sequence":32589047814,"product_id":"ETH-USD","price":"1549.08","open_24h":"1485.35",
"volume_24h":"589548.02768130","low_24h":"1450.45","high_24h":"1631.32","volume_30d":"8147557.57837948",
"best_bid":"1548.81","best_ask":"1549.08","side":"buy","time":"2022-07-19T12:09:26.348930Z","trade_id":319738529,
"last_size":"0.01803274"}

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
	std::string token;
    size_t pos1;
    size_t pos2;
    size_t pos3;
    size_t pos4;
    int count = 0;
    std::cout << "{\n";
    while(count <= 2) {
        if(count == 0) {
            token = "type";
            pos1 = json.find(token);
        } 
        else if(count == 1) {
            token = "price";
            pos1 = json.find(token);
        } 
        else if(count == 2) {
            token = "last_size";
            pos1 = json.find(token);
        }
    	pos2 = json.find(":", pos1+1);
    	pos3 = json.find("\"", pos2+1);
    	pos4 = json.find("\"", pos3+1);
    	std::cout << "    " << token << ": " << json.substr(pos3+1, (pos4 - pos3-1)) << std::endl;
        pos2 = pos3 = pos4 = 0;
        count ++;
    }
    std::cout << "}" << std::endl;
}

inline void parseTickerSsOptm(const std::string &json) {
	std::string token;
    size_t pos1;
    size_t pos2;
    size_t pos3;
    size_t pos4;
    int count = 0;
    while(count <= 2) {
        if(count == 0) {
            token = "type";
            pos1 = json.find(token);
        } 
        else if(count == 1) {
            token = "price";
            pos1 = json.find(token);
        } 
        else if(count == 2) {
            token = "last_size";
            pos1 = json.find(token);
        }
    	pos2 = json.find(":", pos1+1);
    	pos3 = json.find("\"", pos2+1);
    	pos4 = json.find("\"", pos3+1);
    	strpnt(json.c_str(), pos1, pos4);
        pos2 = pos3 = pos4 = 0;
        count ++;
    }
}

// give all values in ticker in simple

/* 
inline void parseTickerSs(const std::string &json) {
	size_t len = json.length();
	char ch;
    int idx = 0;
	int begin = idx;
	do {
		ch = json[idx];
		if (::isspace(ch))
            continue;
		else if (ch == '[' || ch == '{') {
			std::cout << json[idx] << std::endl;
			begin = idx+1;
		} 
        else if (ch == ',' ) {
			std::cout << json.substr(begin, idx-begin) << std::endl;
			begin = idx+1;
		} 
        else if (ch == '}' || ch == ']') {
			std::cout  << json.substr(begin, idx-begin) << std::endl;
			std::cout << ch  << std::endl;
			begin = idx+1;
		} 
    }  while ( ++idx <= len ); 
}
*/

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
	    } 
        else if (ch == '[') {
	    	std::cout << json.substr(begin, idx-begin);
	    	std::cout << json[idx];
	    	begin = idx+1;
	    } 
        else if (ch == ',' ) {
            if(begin >= 51) {
                std::cout << json.substr(begin, idx-begin) << ",";
	    	    begin = idx+1;
            }
            else {
                std::cout << json.substr(begin, idx-begin) << std::endl;
	    	    begin = idx+1;
            }
        } 
        else if (ch == ']') {
	    	std::cout  << json.substr(begin, idx-begin);
	    	std::cout << ch  ;
	    	begin = idx+1;
	    } 
        else if (ch == '}') {
	    	std::cout  << json.substr(begin, idx-begin) << std::endl;
	    	std::cout << ch  ;
	    	begin = idx+1;
            if(++idx >= len)
                std::cout << std::endl;
        } 
    }  while ( ++idx <= len ); 
}

inline void parseSnapshotSsOptm(const std::string& json) { 
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
        if(firstPos == string::npos || secondPos == string::npos || secondPos == arr.size()-1) {
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
        if(firstPos == string::npos || secondPos == string::npos || secondPos == arr.size()-1) {
            break;
        }
        std::string innerArr = arr.substr(firstPos, secondPos-firstPos+1);
        int comma = innerArr.find("\",");
        price = innerArr.substr(2, comma-2); 
        amount = innerArr.substr(comma+3, (innerArr.size() - innerArr.find("\"", comma+2))-3);
        std::cout << price << ", " << amount << std::endl;
    }
}
