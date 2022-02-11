#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

using boost::property_tree::ptree;

void PrintNodes(boost::property_tree::ptree const& pt, std::string pad ="") {
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << pad << "'" << it->first << "': " << it->second.get_value<std::string>() << std::endl;
        if (it->second.begin() != it->second.end()) 
             PrintNodes(it->second, pad+"\t");
    }   
    std::cout << pad << "Done." << std::endl;
}

void FormatNodes(boost::property_tree::ptree const& node, std::stringstream &ss) {
    using boost::property_tree::ptree;
    ptree::const_iterator end = node.end();
    for (ptree::const_iterator it = node.begin(); it != end; ++it) {
        if (it->second.begin() == it->second.end()) 
            ss << "'" << it->first << "'= " << it->second.get_value<std::string>() << ',';
        else
            FormatNodes(it->second, ss);
    }   
}

void HandleOrder(boost::property_tree::ptree const& info) {
    auto result = info.get_child_optional("result");
    if (!result) {
        std::cout << "result not found" << std::endl;
        return; 
    }
    
    auto order = result->get_child_optional("order");
    if (order) {
        std::cout << "NEW: ";
        std::cout << "order_id: " << order->get<std::string>("order_id") << ", ";
        std::cout << "label: " << order->get<std::string>("label") << ", ";
        std::cout << "price: " << order->get<std::string>("price") << ", ";
        std::cout << "amount: " << order->get<std::string>("amount") << ", ";
        std::cout << "filled_amount: " << order->get<std::string>("filled_amount") << ", ";
        std::cout << "direction: " << order->get<std::string>("direction") << "\n";
	std::cout << std::endl;
    }
    
    using boost::property_tree::ptree;
    auto trades = result->get_child_optional("trades");
    if (trades) {
        ptree::const_iterator end = trades->end();
        for (ptree::const_iterator it = trades->begin(); it != end; ++it) {
            std::cout << "TRD: ";
            std::cout << "order_id: " << it->second.get<std::string>("order_id") << ", ";
            std::cout << "label: " << it->second.get<std::string>("label") << ", ";
            std::cout << "price: " << it->second.get<std::string>("price") << ", ";
            std::cout << "filled amount: " << it->second.get<std::string>("amount") << ", ";
            std::cout << "direction: " << it->second.get<std::string>("direction") << "\n";
	    std::cout << std::endl;
        }
    }
}

// New Order Response
// {"jsonrpc":"2.0","id":7431,"result":{"trades":[],"order":{"web":false,"time_in_force":"good_til_cancelled","replaced":false,"reduce_only":false,"profit_loss":0.0,"price":68000.0,"post_only":false,"order_type":"limit","order_state":"open","order_id":"6791515132","max_show":680.0,"last_update_timestamp":1635317754767,"label":"9so7vcq6","is_liquidation":false,"instrument_name":"BTC-PERPETUAL","filled_amount":0.0,"direction":"sell","creation_timestamp":1635317754767,"commission":0.0,"average_price":0.0,"api":true,"amount":680.0}},"usIn":1635317754766998,"usOut":1635317754768243,"usDiff":1245,"testnet":true}
//{"jsonrpc":"2.0","id":6,"result":{
// "trades":[{"trade_seq": 57071736,"trade_id": "84806005","timestamp": 1635044135734,"tick_direction": 0,"state": "filled","self_trade": false,"reduce_only": false,"profit_loss": 0,"price": 61137.5,"post_only": false,"order_type": "limit","order_id": "6773679687","matching_id": null,"mark_price": 61130.79,"liquidity": "T","label": "COID_1001","instrument_name": "BTC-PERPETUAL","index_price": 61085.51,"fee_currency": "BTC","fee": 0.00000818,"direction": "buy","amount": 1000}],
// "order":{"web": false,"time_in_force": "good_til_cancelled","replaced": false,"reduce_only": false,"profit_loss": 0,"price": 61500,"post_only": false,"order_type": "limit","order_state": "filled","order_id": "6773679687","max_show": 1000,"last_update_timestamp": 1635044135734,"label": "COID_1001","is_liquidation": false,"instrument_name": "BTC-PERPETUAL","filled_amount": 1000,"direction": "buy","creation_timestamp": 1635044135734,"commission": 0.00000818,"average_price": 61137.5,"api": true,"amount": 1000}},
//"usIn": 1635044135733570,"usOut": 1635044135735180,"usDiff": 1610,"testnet": true}
int main() {
    std::string aMsg = R"(
{"jsonrpc":"2.0","id":7431,"result":{"trades":[],"order":{"web":false,"time_in_force":"good_til_cancelled","replaced":false,"reduce_only":false,"profit_loss":0.0,"price":68000.0,"post_only":false,"order_type":"limit","order_state":"open","order_id":"6791515132","max_show":680.0,"last_update_timestamp":1635317754767,"label":"9so7vcq6","is_liquidation":false,"instrument_name":"BTC-PERPETUAL","filled_amount":0.0,"direction":"sell","creation_timestamp":1635317754767,"commission":0.0,"average_price":0.0,"api":true,"amount":680.0}},"usIn":1635317754766998,"usOut":1635317754768243,"usDiff":1245,"testnet":true}
)"; 

    // result -> order -> (price, instrument_name, direction, amount, label, order_id)
    // result -> trades -> [(price, instrument_name, direction, amount, label, order_id)]
    ptree info;
    std::istringstream is(aMsg);
    read_json(is, info);
    PrintNodes(info);
    HandleOrder(info);

    aMsg = R"(
{"jsonrpc":"2.0","id":6,"result":{
 "trades":[{"trade_seq": 57071736,"trade_id": "84806005","timestamp": 1635044135734,"tick_direction": 0,"state": "filled","self_trade": false,"reduce_only": false,"profit_loss": 0,"price": 61137.5,"post_only": false,"order_type": "limit","order_id": "6773679687","matching_id": null,"mark_price": 61130.79,"liquidity": "T","label": "COID_1001","instrument_name": "BTC-PERPETUAL","index_price": 61085.51,"fee_currency": "BTC","fee": 0.00000818,"direction": "buy","amount": 1000}],
 "order":{"web": false,"time_in_force": "good_til_cancelled","replaced": false,"reduce_only": false,"profit_loss": 0,"price": 61500,"post_only": false,"order_type": "limit","order_state": "filled","order_id": "6773679687","max_show": 1000,"last_update_timestamp": 1635044135734,"label": "COID_1001","is_liquidation": false,"instrument_name": "BTC-PERPETUAL","filled_amount": 1000,"direction": "buy","creation_timestamp": 1635044135734,"commission": 0.00000818,"average_price": 61137.5,"api": true,"amount": 1000}},
"usIn": 1635044135733570,"usOut": 1635044135735180,"usDiff": 1610,"testnet": true}
)"; 
    is.str(aMsg);
    read_json(is, info);
    PrintNodes(info);
    HandleOrder(info);
}


