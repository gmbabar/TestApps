#include <string>
/*
    {
        "type":"ticker","sequence":32589047814,"product_id":"ETH-USD",
        "price":"1549.08","open_24h":"1485.35","volume_24h":"589548.02768130",
        "low_24h":"1450.45","high_24h":"1631.32","volume_30d":"8147557.57837948",
        "best_bid":"1548.81","best_ask":"1549.08","side":"buy",
        "time":"2022-07-19T12:09:26.348930Z","trade_id":319738529,"last_size":"0.01803274"
    }
*/
inline void parseMsg(const std::string &json, std::string &result) {
    int start = json.find("\"type\"");
    start = json.find(":", start);
    int stop = json.find("\",", start);
    auto type = json.substr(start+2, stop-start-2);

    start = json.find("\"sequence\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto sequence = json.substr(start+2, stop-start-2);

    start = json.find("\"product_id\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto symbol = json.substr(start+2, stop-start-2);

    start = json.find("\"price\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto px = json.substr(start+2, stop-start-2);

    start = json.find("\"open_24h\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto dailyOpens = json.substr(start+2, stop-start-2);

    start = json.find("\"volume_24h\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto dailyVolume = json.substr(start+2, stop-start-2);

    start = json.find("\"low_24h\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto dailyLow = json.substr(start+2, stop-start-2);

    start = json.find("\"high_24h\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto dailyHigh = json.substr(start+2, stop-start-2);

    start = json.find("\"volume_30d\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto monthlyVolume = json.substr(start+2, stop-start-2);

    start = json.find("\"best_bid\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto bestBid = json.substr(start+2, stop-start-2);

    start = json.find("\"best_ask\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto bestAsk = json.substr(start+2, stop-start-2);

    start = json.find("\"side\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto side = json.substr(start+2, stop-start-2);

    start = json.find("\"time\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto time = json.substr(start+2, stop-start-2);

    start = json.find("\"trade_id\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    auto tradeId = json.substr(start+2, stop-start-2);

    start = json.find("\"last_size\"");
    start = json.find(":", start);
    stop = json.find("\"}", start);
    auto lastSize = json.substr(start+2, stop-start-2);
}