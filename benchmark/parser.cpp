#include <chrono>
#include <iostream>
#include <queue>
#include "parser.hpp"

int main()
{
    std::string json = R"({"type":"ticker","sequence":32589047814,"product_id":"ETH-USD","price":"1549.08","open_24h":"1485.35","volume_24h":"589548.02768130","low_24h":"1450.45","high_24h":"1631.32","volume_30d":"8147557.57837948","best_bid":"1548.81","best_ask":"1549.08","side":"buy","time":"2022-07-19T12:09:26.348930Z","trade_id":319738529,"last_size":"0.01803274"})";
    std::string result;
    auto t1 = std::chrono::steady_clock::now();
    for(int i=0; i<1E6; i++) {
        parseMsg(json, result);
    }
    auto t2 = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
    std::cout << "[INFO] Time Taken By Parser to Parse 1M messages is : " << time << " Milliseconds" << std::endl;
}