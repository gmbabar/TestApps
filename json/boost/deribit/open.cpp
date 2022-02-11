#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

using boost::property_tree::ptree;

void PrintNodes(boost::property_tree::ptree const& pt, std::string pad ="") {
    auto end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << pad << "'" << it->first << "': " << it->second.get_value<std::string>() << std::endl;
        if (it->second.begin() != it->second.end()) 
             PrintNodes(it->second, pad+"\t");
    }   
    std::cout << pad << "Done." << std::endl;
}

void ProcessOpenOrder(boost::property_tree::ptree const& info) {
    auto result = info.get_child_optional("result");
    if (!result) {
        std::cout << "result not found" << std::endl;
        return; 
    }   
    
    //for (auto &it : *result) {
    for (auto it = result->begin(); it != result->end(); ++it) {
        std::cout << "ORDER: ";
        std::cout << "instrument: " << it->second.get<std::string>("instrument_name") << ", ";
        std::cout << "order_id: " << it->second.get<std::string>("order_id") << ", ";
        std::cout << "label: " << it->second.get<std::string>("label") << ", ";
        std::cout << "price: " << it->second.get<std::string>("price") << ", ";
        std::cout << "amount: " << it->second.get<std::string>("amount") << ", ";
        std::cout << "filled_amount: " << it->second.get<std::string>("filled_amount") << ", ";
        std::cout << "direction: " << it->second.get<std::string>("direction") << "\n";
        std::cout << std::endl;
    }
}

int main() {
    std::string aMsg = R"(
{
    "jsonrpc": "2.0", "id": 8442, "result": [
        { "time_in_force": "good_til_cancelled", "reduce_only": false, "profit_loss": 0, "price": 0.135, "post_only": false,
          "order_type": "limit", "order_state": "open", "order_id": "ETH-252017", "max_show": 10, "last_update_timestamp": 1550050594882,
          "label": "", "is_liquidation": false, "instrument_name": "ETH-22FEB19-120-C", "filled_amount": 0, "direction": "sell",
          "creation_timestamp": 1550050594882, "commission": 0, "average_price": 0, "api": true, "amount": 10
        }
    ]
}
)"; 
    ptree info;
    std::istringstream is(aMsg);
    read_json(is, info);
    PrintNodes(info);
    ProcessOpenOrder(info);
}


