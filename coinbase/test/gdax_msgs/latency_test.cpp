// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <chrono>

using namespace rapidjson;
using namespace std::chrono;

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
    document["type"].GetString();
    // std::cout << "type: " << document["type"].GetString() << std::endl;
    document["product_id"].GetString();
    // std::cout << "product_id: " << document["product_id"].GetString() << std::endl;
    auto var = document["changes"].GetArray();
    // std::cout << "changes: [";
    for (int idx=0; idx<var.Size(); ++idx) {
        auto &arrVal = var[idx];
        if (arrVal.IsArray()) {
            auto jsonArr = arrVal.GetArray();
            for (int x=0; x<jsonArr.Size(); ++x) {
                jsonArr[x].GetString();
                // std::cout << jsonArr[x].GetString() << ",";
            }
        }
    }
    // std::cout << "\b]" << std::endl;
    document["time"].GetString();
    // std::cout << "time: " << document["time"].GetString() << std::endl;
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
	    	json[idx];
	    	begin = idx+1;
	    } 
        else if (ch == '[') {
	    	json.substr(begin, idx-begin);
	    	json[idx];
	    	begin = idx+1;
	    } 
        else if (ch == ',' ) {
            if(begin >= 53 && begin <= 70) {
                json.substr(begin, idx-begin);
	    	    begin = idx+1;
            } 
            else {
                json.substr(begin, idx-begin);
	    	    begin = idx+1;
            }
        } 
        else if (ch == ']') {
	    	json.substr(begin, idx-begin);
	    	// std::cout << ch;
	    	begin = idx+1;
	    } 
        else if (ch == '}') {
	    	json.substr(begin, idx-begin);
	    	// std::cout << ch;
	    	begin = idx+1;
            if(++idx >= len);
                // std::cout << std::endl;
	    } 
    }  while ( ++idx <= len ); 
}
/*
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
*/
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

inline void parseTicker(const std::string& json) {
    Document document;
    document.Parse(json.c_str());
    document["type"].GetString();
    document["price"].GetString();
    document["last_size"].GetString();       
}

inline void parseTickerSs(const std::string &json) {
	std::string token;
    size_t pos1;
    size_t pos2;
    size_t pos3;
    size_t pos4;
    int count = 0;
    // std::cout << "{\n";
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
    	json.substr(pos3+1, (pos4 - pos3-1));
        pos2 = pos3 = pos4 = 0;
        count ++;
    }
    // std::cout << "}" << std::endl;
}

/*
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
*/

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
----Snapshot
{"type":"snapshot","product_id":"ETH-USD","asks":[["89","0009.989"],["34","0.9879"],["69","0.699696"]],
bids":[["89","0009.989"],["34","0.9879"],["69","0.699696"]]}

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

inline void parseSnapshot(const std::string& json) {
    Document document;
    document.Parse(json.c_str());
    document["type"].GetString();
    document["product_id"].GetString();
    //Gets The Value Of Asks
    auto asks = document["asks"].GetArray();
    // std::cout << "Asks : [" << std::endl;
    for (SizeType i = 0; i < asks.Size(); i++) {
        auto arrVal = asks[i].GetArray();
        // std::cout << "Asks : [";
        for(SizeType i = 0; i < arrVal.Size(); i++)
        {
            auto val = arrVal[i].GetString();
            // std::cout << val << ",";
        }
        // std::cout << "\b]" << std::endl;
    }
    // std::cout << "]" << std::endl;
    
    auto bids = document["bids"].GetArray();
    // std::cout << "bids : [" << std::endl;
    for (SizeType i = 0; i < bids.Size(); i++) {
        auto arrVal = bids[i].GetArray();
        // std::cout << "Bids : [";
        for(SizeType i = 0; i < arrVal.Size(); i++) {
            auto val = arrVal[i].GetString();
            // std::cout << val << ",";
        }
        // std::cout << "\b]" << std::endl;
    }
    // std::cout << "]" << std::endl;
}

/*
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
*/
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
			json[idx];
			begin = idx+1;
	    } 
        else if (ch == '[') {
	    	json.substr(begin, idx-begin);
	    	json[idx];
	    	begin = idx+1;
	    } 
        else if (ch == ',' ) {
            if(begin >= 51) {
                json.substr(begin, idx-begin) ;
	    	    begin = idx+1;
            }
            else {
                json.substr(begin, idx-begin);
	    	    begin = idx+1;
            }
        } 
        else if (ch == ']') {
	    	json.substr(begin, idx-begin);
	    	begin = idx+1;
	    } 
        else if (ch == '}') {
	        json.substr(begin, idx-begin);
	    	
	    	begin = idx+1;
            if(++idx >= len);
        } 
    }  while ( ++idx <= len ); 
}

int main() {
    std::ostringstream oss;
    int count = 1000000;
    std::cout << "----------------------- Snapshot Msg -----------------------" << std::endl;
    formatSnapshotMsg(oss, "snapshot", "ETH-USD",R"([["89","0009.989"],["34","0.9879"],["69","0.699696"]])",R"([["89","0009.989"],["34","0.9879"],["69","0.699696"]])");
    std::cout << "Json: " << oss.str() << std::endl;
    std::chrono::steady_clock::time_point t1  = std::chrono::steady_clock::now();
    for (size_t i = 0; i < count; i++) {
        parseSnapshotSs(oss.str());
    }
    std::chrono::steady_clock::time_point t2  = std::chrono::steady_clock::now();
    std::chrono::microseconds timetaken = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);
    std::cout << "The total time taken by Ss parser in micro sec is : "<< timetaken.count() << std::endl;

    std::chrono::steady_clock::time_point t3  = std::chrono::steady_clock::now();
    for (size_t i = 0; i < count; i++) {
        parseSnapshot(oss.str());
    }
    std::chrono::steady_clock::time_point t4  = std::chrono::steady_clock::now();
    std::chrono::microseconds timetaken2 = std::chrono::duration_cast<std::chrono::microseconds>(t4-t3);
    std::cout << "The total time taken by rapidjson parser in micro sec is : "<< timetaken2.count() << std::endl;

    oss.str("");

    std::cout << "----------------------- L2update Msg -----------------------" << std::endl;
    formatL2updateMsg(oss,"l2update","ETHUSD",R"([["buy","1550.16","0.80679000"]])","2022-07-19T12:09:18.390473Z");
    std::cout << "Json: " << oss.str() << std::endl;
    parseL2updateSs(oss.str());
    parseL2update(oss.str());
    std::chrono::steady_clock::time_point t5  = std::chrono::steady_clock::now();
    for (size_t i = 0; i < count; i++) {
        parseL2updateSs(oss.str());
    }
    std::chrono::steady_clock::time_point t6  = std::chrono::steady_clock::now();
    std::chrono::microseconds timetaken3 = std::chrono::duration_cast<std::chrono::microseconds>(t6-t5);
    std::cout << "The total time taken by Ss parser in micro sec is : "<< timetaken3.count() << std::endl;

    std::chrono::steady_clock::time_point t7  = std::chrono::steady_clock::now();
    for (size_t i = 0; i < count; i++) {
        parseL2update(oss.str());
    }
    std::chrono::steady_clock::time_point t8  = std::chrono::steady_clock::now();
    std::chrono::microseconds timetaken4 = std::chrono::duration_cast<std::chrono::microseconds>(t8-t7);
    std::cout << "The total time taken by rapidjson parser in micro sec is : "<< timetaken4.count() << std::endl;
    oss.str("");
    
    std::cout << "----------------------- Ticker Msg -----------------------" << std::endl;
    formatTickerMsg(oss, "ticker",32589047814, "ETH-USD","1549.08","1485.35","589548.02768130","1450.45","1631.32",
                "8147557.57837948","1548.81","1549.08","buy","2022-07-19T12:09:26.348930Z",319738529,"0.01803274");
    std::cout << "Json: " << oss.str() << std::endl;
     std::chrono::steady_clock::time_point t9  = std::chrono::steady_clock::now();
    for (size_t i = 0; i < count; i++) {
        parseTickerSs(oss.str());
    }
    std::chrono::steady_clock::time_point t10  = std::chrono::steady_clock::now();
    std::chrono::microseconds timetaken5 = std::chrono::duration_cast<std::chrono::microseconds>(t10-t9);
    std::cout << "The total time taken by Ss parser in micro sec is : "<< timetaken5.count() << std::endl;
    std::chrono::steady_clock::time_point t11  = std::chrono::steady_clock::now();
    for (size_t i = 0; i < count; i++) {
        parseTicker(oss.str());  
    }
    std::chrono::steady_clock::time_point t12  = std::chrono::steady_clock::now();
    std::chrono::microseconds timetaken6 = std::chrono::duration_cast<std::chrono::microseconds>(t12-t11);
    std::cout << "The total time taken by rapid json parser in micro sec is : "<< timetaken6.count() << std::endl;

    oss.str("");

    return 0;
}

