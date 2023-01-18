#include "server.hpp"

int main() {
	std::string msg = R"({
        "type":"ticker","sequence":32589047814,"product_id":"ETH-USD",
        "price":"1549.08","open_24h":"1485.35","volume_24h":"589548.02768130",
        "low_24h":"1450.45","high_24h":"1631.32","volume_30d":"8147557.57837948",
        "best_bid":"1548.81","best_ask":"1549.08","side":"buy",
        "time":"2022-07-19T12:09:26.348930Z","trade_id":319738529,"last_size":"0.01803274"
    })";
	int port = 6969;
	long sendTime = 1E6;
	Server server(port, sendTime, msg);
	server.start();
}