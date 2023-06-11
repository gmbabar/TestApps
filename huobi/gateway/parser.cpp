#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

void ParseBalance(const std::string& jsonString) {
    boost::property_tree::ptree pt;
    std::istringstream jsonStream(jsonString);
    
    try {
        boost::property_tree::read_json(jsonStream, pt);
        
        std::string action = pt.get<std::string>("action");
        std::string ch = pt.get<std::string>("ch");
        std::string currency = pt.get<std::string>("data.currency");
        int accountId = pt.get<int>("data.accountId");
        std::string balance = pt.get<std::string>("data.balance");
        std::string changeType = pt.get<std::string>("data.changeType");
        std::string accountType = pt.get<std::string>("data.accountType");
        std::string seqNum = pt.get<std::string>("data.seqNum");
        long long changeTime = pt.get<long long>("data.changeTime");

        std::cout << "action: " << action << std::endl;
        std::cout << "ch: " << ch << std::endl;
        std::cout << "currency: " << currency << std::endl;
        std::cout << "accountId: " << accountId << std::endl;
        std::cout << "balance: " << balance << std::endl;
        std::cout << "changeType: " << changeType << std::endl;
        std::cout << "accountType: " << accountType << std::endl;
        std::cout << "seqNum: " << seqNum << std::endl;
        std::cout << "changeTime: " << changeTime << std::endl;
    } catch (const boost::property_tree::ptree_error& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
}

// msg = {
//         "action": "sub",
//         "ch": channel
//     }

struct authInfo {
    std::string pubKey;
    std::string priKey;
};

inline std::string fmtHuobiWebsocketSubscribeChannel(const std::string &channel) {

    boost::property_tree::ptree json;
    json.put("action", "sub");
    json.put("ch", channel);
    std::stringstream ss;
    boost::property_tree::write_json(ss, json);
    return ss.str();
}

int main() {
    std::string jsonString = R"({"action": "push", "ch": "accounts.update#0", "data": {"currency": "btc", "accountId": 123456, "balance": "23.111", "changeType": "transfer", "accountType": "trade", "seqNum": "86872993928", "changeTime": 1568601800000}})";
    const auto json = fmtHuobiWebsocketSubscribeChannel("accounts.update");
    std::cout << json << std::endl;
    ParseBalance(jsonString);

    return 0;
}
