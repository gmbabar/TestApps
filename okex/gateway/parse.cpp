#include <iostream>
#include <string>
#include "rapidjson/document.h"
#include <boost/beast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace beast = boost::beast;
namespace pt = boost::property_tree;

using namespace rapidjson;

void extractBalData(const std::string& message) {
    Document document;
    document.Parse(message.c_str());

    if (!document.IsObject()) {
        std::cerr << "Invalid JSON format" << std::endl;
        return;
    }

    const Value& data = document["data"];

    if (!data.IsArray() || data.Empty()) {
        std::cerr << "Invalid data format" << std::endl;
        return;
    }

    const Value& balData = data[0]["balData"];

    if (!balData.IsArray()) {
        std::cerr << "Invalid balData format" << std::endl;
        return;
    }

    for (SizeType i = 0; i < balData.Size(); ++i) {
        const Value& entry = balData[i];

        if (!entry.IsObject()) {
            std::cerr << "Invalid entry format" << std::endl;
            continue;
        }

        const Value& cashBal = entry["cashBal"];
        const Value& ccy = entry["ccy"];
        const Value& uTime = entry["uTime"];

        if (!cashBal.IsString() || !ccy.IsString() || !uTime.IsString()) {
            std::cerr << "Invalid entry fields" << std::endl;
            continue;
        }

        std::cout << "cashBal: " << cashBal.GetString() << std::endl;
        std::cout << "ccy: " << ccy.GetString() << std::endl;
        std::cout << "uTime: " << uTime.GetString() << std::endl;
        std::cout << std::endl;
    }
}


void boostExtractBalData(const std::string& message) {
    // Parse the JSON message
    pt::ptree tree;
    std::istringstream iss(message);
    pt::read_json(iss, tree);

    // Extract "balData" information
    const auto& balData = tree.get_child("data").front().second.get_child("balData");

    // Iterate over "balData" elements
    for (const auto& entry : balData) {
        const std::string cashBal = entry.second.get<std::string>("cashBal");
        const std::string ccy = entry.second.get<std::string>("ccy");
        const std::string uTime = entry.second.get<std::string>("uTime");

        // Print the extracted information
        std::cout << "cashBal: " << cashBal << std::endl;
        std::cout << "ccy: " << ccy << std::endl;
        std::cout << "uTime: " << uTime << std::endl;
        std::cout << std::endl;
    }
}

int main() {
    std::string message = R"({"arg":{"channel":"balance_and_position","uid":"441926842908621946"},"data":[{"balData":[{"cashBal":"3.00019729251","ccy":"BTC","uTime":"1685373934805"},{"cashBal":"30","ccy":"LTC","uTime":"1683628111356"},{"cashBal":"9000","ccy":"TUSD","uTime":"1683628111330"},{"cashBal":"15","ccy":"ETH","uTime":"1683628111343"},{"cashBal":"3000","ccy":"ADA","uTime":"1683628111390"},{"cashBal":"9000","ccy":"USDK","uTime":"1683628111239"},{"cashBal":"8994.620934701","ccy":"USDT","uTime":"1685373934805"},{"cashBal":"1500","ccy":"UNI","uTime":"1683628111318"},{"cashBal":"300","ccy":"JFI","uTime":"1683628111292"},{"cashBal":"30000","ccy":"TRX","uTime":"1683628111379"},{"cashBal":"300","ccy":"OKB","uTime":"1683628111266"},{"cashBal":"9000","ccy":"USDC","uTime":"1683628111367"},{"cashBal":"9000","ccy":"PAX","uTime":"1683628111255"}],"eventType":"snapshot","pTime":"1685376434023","posData":[]}]})";

    // extractBalData(message);
    boostExtractBalData(message);

    return 0;
}
