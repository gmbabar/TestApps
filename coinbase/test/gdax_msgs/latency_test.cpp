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
    // std::cout << "type: " << document["type"].GetString() << std::endl;
    document["type"].GetString();
    document["product_id"].GetString() ;
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
    // std::cout << "time: " << document["time"].GetString() << std::endl;
}

inline void parseL2updateSs(const std::string& json) {

    int start = json.find("\"changes\"");
    start = json.find(":", start);
    std::string arr = json.substr(start+1, json.find("]]")-start+1);

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
        // std::cout << side << ", " << price << ", " << amount << std::endl;
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



// only give the required values

inline void parseTicker(const char *json) {
    Document document;
    document.Parse(json);
    document["type"].GetString();
    // std::cout << "type: " << document["type"].GetString() << std::endl;
    document["price"].GetString();
    // std::cout << "price: " << document["price"].GetString() << std::endl;
    document["last_size"].GetString();    
    // std::cout << "last_size: " << document["last_size"].GetString() << std::endl;       
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
    // std::cout << "type: " << type << std::endl;
    // std::cout << "Price: " << px << std::endl;
    // std::cout << "last_size: " << amt << std::endl;
}


/*
----Snapshot
{"type":"snapshot","product_id":"ETH-USD","asks":[["89","0009.989"],["34","0.9879"],["69","0.699696"]],bids":[["89","0009.989"],["34","0.9879"],["69","0.699696"]]}

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

inline void parseSnapshot(const char *json) {
    Document document;
    document.Parse(json);
    document["type"].GetString();
    // std::cout << "Type : " << document["type"].GetString() << std::endl;
    document["product_id"].GetString();
    // std::cout << "product Id : " << document["product_id"].GetString() << std::endl;
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


inline void parseSnapshotSs(const std::string& json) { 
    int firstPos = 0;
    int secondPos = 0;
    std::string price;
    std::string amount;
    int start = json.find("\"type\"");
    start = json.find(":", start);
    int stop  = json.find("\",", start);
    // std::cout << json.substr(start+2, stop-start-2) << std::endl;
    start = json.find("\"product_id\"");
    start = json.find(":", start);
    stop  = json.find("\",", start);
    // std::cout << json.substr(start+2, stop-start-2) << std::endl;
    start = json.find("\"asks\"");
    start = json.find(":", start);
    stop  = json.find("]],", start);
    std::string arr = json.substr(start+1, stop-start+1);
    // std::cout << "ASKS : " << arr << std::endl;
    while (true) {
        firstPos = arr.find("[", firstPos+1);
        secondPos = arr.find("]", secondPos+1);
        if(firstPos == std::string::npos || secondPos == std::string::npos || secondPos == arr.size()-1) {
            break;
        }
        std::string innerArr = arr.substr(firstPos, secondPos-firstPos+1);
        int comma = innerArr.find("\",");
        price = innerArr.substr(2, comma-2); 
        amount = innerArr.substr(comma+3, (innerArr.size() - innerArr.find("\"", comma+2))-3);
        // std::cout << price << ", " << amount << std::endl;
    }
    firstPos = 0;
    secondPos = 0;
    start = json.find("\"bids\"");
    start = json.find(":", start);
    stop  = json.find("]]", start);
    arr = json.substr(start+1, stop-start+1);
    // std::cout << "BIDS : " << arr << std::endl;
    while (true) {
        firstPos = arr.find("[", firstPos+1);
        secondPos = arr.find("]", secondPos+1);
        if(firstPos == std::string::npos || secondPos == std::string::npos || secondPos == arr.size()-1) {
            break;
        }
        std::string innerArr = arr.substr(firstPos, secondPos-firstPos+1);
        int comma = innerArr.find("\",");
        price = innerArr.substr(2, comma-2); 
        amount = innerArr.substr(comma+3, (innerArr.size() - innerArr.find("\"", comma+2))-3);
        // std::cout << price << ", " << amount << std::endl;
    }
}


int main() {
    std::ostringstream oss;
    int count = 1E6;
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
        parseSnapshot(oss.str().c_str());
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
        parseTicker(oss.str().c_str());  
    }
    std::chrono::steady_clock::time_point t12  = std::chrono::steady_clock::now();
    std::chrono::microseconds timetaken6 = std::chrono::duration_cast<std::chrono::microseconds>(t12-t11);
    std::cout << "The total time taken by rapid json parser in micro sec is : "<< timetaken6.count() << std::endl;
    oss.str("");

    return 0;
}

