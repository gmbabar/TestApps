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
            // for (const auto &val : trade) {
            //     std::cout << pad << val.first << " : " << val.second.get_value<std::string>() << std::endl;
            // }
            const auto px = trade.get<std::string>("price");
            const auto qty = trade.get<std::string>("vol");
            const auto sym = trade.get<std::string>("pair");
            std::cout << px << " | " << qty << " | " << sym << std::endl;

        }
        else if (it->second.begin() != it->second.end()) {
            std::cout << "Printing Trade:" << std::endl;
        	PrintTradeData(it->second, pad+".\t");
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
            const auto event = pt.get<std::string>("event");
            std::cout << "EVENT: " << event << std::endl;
            const auto status = pt.get<std::string>("status");
            std::cout << "STATUS: " << status << std::endl;
            if (status == "error") {
                const auto errorReason = pt.get<std::string>("errorMessage");
                std::cout << "ERROR REASON: " << errorReason << std::endl;
                return;
            }
            if(event == "addOrderStatus") {
                const auto orderId = pt.get<std::string>("txid");
                std::cout << "ORDER ADDED WITH TXID: " << orderId << std::endl;
            }
            else if(event == "cancelOrderStatus") {
                std::cout << "ORDER CANCELLED." << std::endl;
            }

            std::cout << "--------------RESPONSE PARSING DONE--------------" << std::endl;
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

    const std::string newOrderOkay = R"(
{
  "descr": "buy 0.01770000 XBTUSD @ limit 4000",
  "event": "addOrderStatus",
  "status": "ok",
  "txid": "ONPNXH-KMKMU-F4MR5V"
}
    )";
    ProcessMessage(newOrderOkay);

    const std::string newOrderFailed = R"(
{
  "errorMessage": "EOrder:Order minimum not met",
  "event": "addOrderStatus",
  "status": "error"
}
    )";
    ProcessMessage(newOrderFailed);


    const std::string cancelOrderOkay = R"(
{
  "event": "cancelOrderStatus",
  "status": "ok"
}
    )";
    ProcessMessage(cancelOrderOkay);

    const std::string cancelOrderFailed = R"(
{
  "errorMessage": "EOrder:Unknown order",
  "event": "cancelOrderStatus",
  "status": "error"
}
    )";
    ProcessMessage(cancelOrderFailed);


    return 0;
}
