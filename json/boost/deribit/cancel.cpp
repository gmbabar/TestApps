#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::property_tree::ptree;

void PrintNodes(boost::property_tree::ptree const& pt, std::string pad ="") {
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << pad << "'" << it->first << "': " << it->second.get_value<std::string>() << std::endl;
        if (it->second.begin() != it->second.end()) 
             PrintNodes(it->second, pad+"\t");
    }   
    std::cout << pad << "Done." << std::endl;
}

void HandleCancelByLabel(boost::property_tree::ptree const& info) {
    auto result = info.get_child_optional("result");
    if (!result) {
        std::cout << "result not found" << std::endl;
        return; 
    }
    std::cout << "CANCEL_BY_LABEL: " << std::endl;
    std::cout << "result: " << result->get_value<std::string>() << std::endl;
}
    
void HandleCancel(boost::property_tree::ptree const& info) {
    auto result = info.get_child_optional("result");
    if (!result) {
        std::cout << "result not found" << std::endl;
        return; 
    }
    if (result->begin() == result->end())
        return HandleCancelByLabel(info);
    
    // else:
    {
        std::cout << "CANCEL: ";
        std::cout << "order_id: " << result->get<std::string>("order_id") << ", ";
        std::cout << "label: " << result->get<std::string>("label") << ", ";
        std::cout << "price: " << result->get<std::string>("price") << ", ";
        std::cout << "amount: " << result->get<std::string>("amount") << ", ";
        std::cout << "filled_amount: " << result->get<std::string>("filled_amount") << ", ";
        std::cout << "direction: " << result->get<std::string>("direction") << "\n";
        std::cout << "order_state: " << result->get<std::string>("order_state") << "\n";
	std::cout << std::endl;
    }
     
    auto price = result->get<std::string>("price");
    auto execAmt = result->get<std::string>("filled_amount");
    std::stringstream ss;
    ss << boost::lexical_cast<double>(execAmt) / boost::lexical_cast<double>(price);
    auto execQty = ss.str();
    auto clientOrderId = result->get<std::string>("label");
    auto orderId = result->get<std::string>("order_id");
   
    std::cout << "OUT: " << price << ", excAmt: " << execAmt << ", excQty: " << execQty << ", coid: " << clientOrderId << ", oid: " << orderId << std::endl;
}

// //Cancel Order Response
// 1.
// {"jsonrpc": "2.0", "id": 11,
//  "result": { "web": false, "time_in_force": "good_til_cancelled", "replaced": false, "reject_post_only": false, "reduce_only": false, "profit_loss": 0, "price": 58500, "post_only": true, "order_type": "limit", "order_state": "cancelled", "order_id": "6773658503", "max_show": 1000, "last_update_timestamp": 1635045036009, "label": "COID_1001", "is_liquidation": false, "instrument_name": "BTC-PERPETUAL", "filled_amount": 0, "direction": "buy", "creation_timestamp": 1635043805902, "commission": 0, "average_price": 0, "api": true, "amount": 1000 },
//  "usIn": 1635045036009037, "usOut": 1635045036009964, "usDiff": 927, "testnet": true}
// 2. 
// {"jsonrpc": "2.0", "id": 14, "result": 1, "usIn": 1635046482410769, "usOut": 1635046482432166, "usDiff": 21397, "testnet": true}

int main() {
    std::string aMsg = R"(
{ "jsonrpc": "2.0", "id": 11, 
  "result": {
    "web": false, "time_in_force": "good_til_cancelled", "replaced": false, "reject_post_only": false, "reduce_only": false, "profit_loss": 0, "price": 58500,
    "post_only": true, "order_type": "limit", "order_state": "cancelled", "order_id": "6773658503", "max_show": 1000, "last_update_timestamp": 1635045036009,
    "label": "COID_1001", "is_liquidation": false, "instrument_name": "BTC-PERPETUAL", "filled_amount": 0, "direction": "buy", "creation_timestamp": 1635043805902,
    "commission": 0, "average_price": 0, "api": true, "amount": 1000
  },
  "usIn": 1635045036009037, "usOut": 1635045036009964, "usDiff": 927, "testnet": true
}
)"; 

    // result -> order -> (price, instrument_name, direction, amount, label, order_id)
    // result -> trades -> [(price, instrument_name, direction, amount, label, order_id)]
    ptree info;
    std::istringstream is(aMsg);
    read_json(is, info);
    PrintNodes(info);
    HandleCancel(info);

    aMsg = R"(
{"jsonrpc": "2.0", "id": 14, "result": 1, "usIn": 1635046482410769, "usOut": 1635046482432166, "usDiff": 21397, "testnet": true}
)";
    is.str(aMsg);
    read_json(is, info);
    PrintNodes(info);
    HandleCancel(info);
    //HandleCancelByLabel(info);

    aMsg = R"(
{"jsonrpc":"2.0","id":2,"result":{"web":false,"time_in_force":"good_til_cancelled","replaced":false,"reduce_only":false,"profit_loss":0.0,"price":68000.0,"post_only":false,"order_type":"limit","order_state":"cancelled","order_id":"6799566677","max_show":680.0,"last_update_timestamp":1635416695859,"label":"9upvib2z","is_liquidation":false,"instrument_name":"BTC-PERPETUAL","filled_amount":0.0,"direction":"sell","creation_timestamp":1635416679625,"commission":0.0,"average_price":0.0,"api":true,"amount":680.0},"usIn":1635416695858451,"usOut":1635416695859853,"usDiff":1402,"testnet":true}
)";
    is.str(aMsg);
    read_json(is, info);
    PrintNodes(info);
    HandleCancel(info);

}


