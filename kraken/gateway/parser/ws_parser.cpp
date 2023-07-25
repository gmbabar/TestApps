#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

void PrintOrderDetails(boost::property_tree::ptree const& order, std::string pad ="") {
    for (const auto &val : order) {
        std::cout << pad << val.first << " : " << val.second.get_value<std::string>() << std::endl;
        if (val.first == "descr" && val.second.begin() != val.second.end()) {
            // Further Description
            for (const auto &descr : val.second) {
                std::cout << pad << ".\t" << descr.first << " : " << descr.second.get_value<std::string>() << std::endl;
            }
        }
    }
}

void PrintOrderData(boost::property_tree::ptree const& pt, std::string pad ="")
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::string note;
        if (!it->first.empty() && it->second.begin() != it->second.end()) {
            std::cout << pad << "OrderID: " << it->first << ": " << it->second.get_value<std::string>() << std::endl;
            const auto &order = it->second;
            PrintOrderDetails(it->second, pad+".\t");
        }
        else if (it->second.begin() != it->second.end()) {
            std::cout << "Printing order:" << std::endl;
        	PrintOrderData(it->second, pad+".\t");
        }
    }
    std::cout << pad << __func__ << " is Done." << endl;
}

void PrintTradeData(boost::property_tree::ptree const& pt, std::string pad ="") {
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::string note;
        if (!it->first.empty() && it->second.begin() != it->second.end()) {
            std::cout << pad << "TradeID: " << it->first << ": " << it->second.get_value<std::string>() << std::endl;
            const auto &trade = it->second;
            // PrintOrderDetails(it->second, pad+".\t");
            for (const auto &val : trade) {
                std::cout << pad << val.first << " : " << val.second.get_value<std::string>() << std::endl;
            }

        }
        else if (it->second.begin() != it->second.end()) {
            std::cout << "Printing trade:" << std::endl;
        	PrintOrderData(it->second, pad+".\t");
        }
    }
    std::cout << pad << __func__ << " is Done." << endl;
}


void ProcessMessage(const std::string &msg) {
    try {
        boost::property_tree::ptree pt;
        std::istringstream jsonStream(msg);

        // Read the JSON data into the property tree
        boost::property_tree::read_json(jsonStream, pt);

        // Check if the root node is an array or key-value pair
        if (!pt.empty() && pt.begin()->first.empty()) {
            std::cout << "-------------- Start -----------------" << std::endl;
            std::cout << __func__ << ": SIZE: " << pt.size() << std::endl;
            const auto &typeItr = ++pt.begin();
            const auto &msgType = typeItr->second.get_value<std::string>();
            std::cout << __func__ << ": TYPE: " << msgType << std::endl;
            if (msgType == "openOrders") {
                PrintOrderData(pt.begin()->second);
            }
            else if (msgType == "ownTrades") {
                PrintTradeData(pt.begin()->second);
            }
            else {
                std::cout << "Error: UNKNOWN Msg Type" << std::endl;
            }
            
        } else {
            std::cout << "The JSON data is UNKNOWN key-value pair." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    const std::string orderData = R"(
[[
{"OGTT3Y-C6I3P-XRI6HX": {"avg_price": "34.50000", "status": "open", "vol": "10.00345345",
    "descr": {"ordertype": "limit","pair": "XBT/EUR","price": "34.50000","type": "sell"}}},
{"OGTT3Y-C6I3P-XRI6HY": {"status": "closed"}},
{"OGTT3Y-C6I3P-XRI6HZ": {"status": "closed"}}
],
"openOrders",
{"sequence": 59342}
]
    )";
    ProcessMessage(orderData);
    
    const std::string tradeData = R"(
[[
{"TDLH43-DVQXD-2KHVYY": {
  "ordertxid": "TDLH43-DVQXD-2KHVYY","ordertype": "limit","pair": "XBT/USD",
  "postxid": "OGTT3Y-C6I3P-XRI6HX","price": "21900.00","type": "sell",
  "vol": "340.000"
}},
{"TDLH43-DVQXD-2KHVYZ": {
  "ordertxid": "TDLH43-DVQXD-2KHVYZ","ordertype": "limit","pair": "XBT/EUR",
  "postxid": "OGTT3Y-C6I3P-XRI6HX","price": "24500.00000","type": "buy",
  "vol": "214.000"
}}
],
"ownTrades",
{"sequence": 2948}
]
    )";
    ProcessMessage(tradeData);

    return 0;
}
