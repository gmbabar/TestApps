#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <chrono>
#include <sstream>
#include <arpa/inet.h>
#include <cstring>
#include <queue>

using namespace std;
std::queue<std::string> myQueue;

// Gemini Pricefeed Parser.

/*
-------l2-update
{"type":"l2_updates","symbol":"BTCUSD","changes":[["buy","21440.73","0"],["sell","21468.93","0"]]}
*/
inline void fastParseL2update(const std::string& json) {
    size_t firstPos = 0;
    size_t lastPos = 0;
    const char *aBuf = json.c_str();
    std::string price;
    std::string amount;
    std::string side;
    int start = json.find("\"type\"");
    start = json.find(":", start);
    int stop  = json.find("\",", start);
    std::string type(&aBuf[start]+2, stop-start-2);
    // std::cout << type << std::endl;
    start = json.find("\"symbol\"");
    start = json.find(":", start);
    stop  = json.find("\",", start);
    std::string symbol(&aBuf[start]+2, stop-start-2);
    // std::cout << symbol << std::endl;
    start = json.find("\"changes\"");
    start = json.find(":", start);
    stop  = json.find("]]", start);
    const std::string changes(&aBuf[start]+1, stop-start+1);
    // std::string arr = changes;
    // std::cout << "changes : " << changes << std::endl;
    while (true) {
        firstPos = changes.find("[", lastPos+1);
        lastPos = changes.find("]", firstPos+1);
        if(firstPos == std::string::npos || lastPos == std::string::npos || lastPos == changes.size()-1) {
            break;
        }
        std::string update(&changes[firstPos], lastPos+1-firstPos);
        // std::cout << update << std::endl;
        size_t delim = update.find("\",");
        std::string side(&update[2], delim-2);
        start = delim;
        delim = update.find("\",",start+1);
        std::string price(&update[start]+3, delim-start-3);
        start = delim;
        delim = update.find("\"", start+4);
        std::string amount(&update[start]+3, delim-start-3);
        // std::cout << side <<"|"<< price << "| " << amount << std::endl;
    }
}

inline void fastParseTrades(const std::string&json) {
    int start = 0;
    int stop = 0;
    size_t firstPos = 0;
    size_t secondPos = 0;
    const char *aBuf = json.c_str();
    start = json.find("\"trades\"");
    start = json.find(":", start);
    stop  = json.find("}],", start);
    std::string trades(&aBuf[start]+1, stop+2-start-1);
    // std::cout << "trades : " << trades << std::endl;
    while (true) {
        firstPos = trades.find("{", secondPos+1);
        secondPos = trades.find("}", firstPos+1);
        if(firstPos == std::string::npos || secondPos == std::string::npos || secondPos == trades.size()-1) {
            break;
        }
        std::string aTrade(&trades[firstPos], secondPos+1-firstPos);
        // std::cout << aTrade << std::endl;
        start = aTrade.find("\"type\"");
        start = aTrade.find("\"trade\"", start);
        stop  = aTrade.find("\",", start);
        std::string type(&aTrade[start]+2, stop-start-2);

        start = aTrade.find("\"symbol\"");
        start = aTrade.find(":", start);
        stop  = aTrade.find("\",", start);
        std::string symbol(&aTrade[start]+2, stop-start-2);

        start = aTrade.find("\"event_id\"");
        start = aTrade.find(":", start);
        stop  = aTrade.find(",", start);
        std::string event_id(&aTrade[start]+2, stop-start-2);

        start = aTrade.find("\"timestamp\"");
        start = aTrade.find(":", start);
        stop  = aTrade.find(",", start);
        std::string timestamp(&aTrade[start]+2, stop-start-2);

        start = aTrade.find("\"price\"");
        start = aTrade.find(":", start);
        stop  = aTrade.find("\",", start);
        std::string price(&aTrade[start]+2, stop-start-2);

        start = aTrade.find("\"quantity\"");
        start = aTrade.find(":", start);
        stop  = aTrade.find("\",", start);
        std::string quantity(&aTrade[start]+2, stop-start-2);

        start = aTrade.find("\"side\"");
        start = aTrade.find(":", start);
        stop  = aTrade.find("\"}", start);
        std::string side(&aTrade[start]+2, stop-start-2);

        // std::cout << type <<"| "<< symbol << "| " << event_id << "| " << timestamp <<"| "<< price << "| " << quantity <<"| "<< side << std::endl;
    }
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
}

int main()
{
    std::string json = R"(
{"type": "l2_updates","symbol": "BTCUSD",
"changes": [["buy","9122.04","0.00121425"],["sell","9122.07", "0.98942292"]],
"trades": [{"type": "trade","symbol": "BTCUSD","event_id": 169841458,"timestamp": 1560976400428,"price": "9122.04","quantity": "0.0073173","side": "sell"}],
"auction_events":[{"type":"auction_result","symbol":"BTCUSD","time_ms":1656100800000,"result":"failure","highest_bid_price":"21241.05","lowest_ask_price":"21246.50"},
{"type":"auction_indicative","symbol":"BTCUSD","time_ms":1656100755000,"result":"failure","highest_bid_price":"21228.83","lowest_ask_price":"21234.33","collar_price":"21231.58"}
})";
    auto t1 = std::chrono::steady_clock::now();
    for(int i=0; i<1E6; i++) {
        fastParseV2Marketdata(json);
    }
    auto t2 = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
    std::cout << "[INFO] Time Taken By PARSER to PARSE 1M messages is : " << time << " Milliseconds The Size Of Queue Was " << myQueue.size() << std::endl;
}