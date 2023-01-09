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
	myQueue.push(type);
	myQueue.push(px);
	myQueue.push(amt);
    // std::cout << "type: " << type << std::endl;
    // std::cout << "Price: " << px << std::endl;
    // std::cout << "last_size: " << amt << std::endl;
}

int main()
{   
    std::string json = R"({"type":"ticker","sequence":32589047814,"product_id":"ETH-USD","price":"1549.08","open_24h":"1485.35","volume_24h":"589548.02768130","low_24h":"1450.45","high_24h":"1631.32","volume_30d":"8147557.57837948","best_bid":"1548.81","best_ask":"1549.08","side":"buy","time":"2022-07-19T12:09:26.348930Z","trade_id":319738529,"last_size":"0.01803274"})";
    auto t1 = std::chrono::steady_clock::now();
    for(int i=0; i<1E6; i++) {
        parseTickerSs(json);
    }
    auto t2 = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
    std::cout << "[INFO] Time Taken By SERVER to SEND 1M messages is : " << time << " Milliseconds The Size Of Queue Was " << myQueue.size() << std::endl;
}