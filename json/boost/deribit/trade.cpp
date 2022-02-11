#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::property_tree::ptree;

void PrintNodes(boost::property_tree::ptree const& pt, std::string pad ="")
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << pad << "'" << it->first << "': " << it->second.get_value<std::string>() << std::endl;
        if (it->second.begin() != it->second.end()) 
             PrintNodes(it->second, pad+"\t");
    }   
    std::cout << pad << "Done." << std::endl;
}

void ProcessTrade(boost::property_tree::ptree const& pt) {
    auto result = pt.get_child_optional("result");
    if (!result) {
        std::cout << "result not found" << std::endl;
        return;
    }
    auto trades = result->get_child_optional("trades");
    if (trades) {
        ptree::const_iterator end = trades->end();
        for (ptree::const_iterator trdit = trades->begin(); trdit != end; ++trdit) {
            std::cout << "TRD: ";
            std::cout << "order_id: " << trdit->second.get<std::string>("order_id") << ", ";
            std::cout << "label: " << trdit->second.get<std::string>("label") << ", ";
            std::cout << "price: " << trdit->second.get<std::string>("price") << ", ";
            std::cout << "amount: " << trdit->second.get<std::string>("amount") << ", ";
            std::cout << "fill_id: " << trdit->second.get<std::string>("trade_id") << ", ";
            std::cout << "liquidity: " << trdit->second.get<std::string>("liquidity") << ", ";
            std::cout << "direction: " << trdit->second.get<std::string>("direction") << "\n";

            auto exchID = trdit->second.get<std::string>("order_id");
            auto clientOrderId = trdit->second.get<std::string>("label");
            auto amount = trdit->second.get<std::string>("amount");
            auto fillID = trdit->second.get<std::string>("trade_id");
            bool maker = (trdit->second.get<std::string>("liquidity") == "M");
            auto exchFillPx = trdit->second.get<std::string>("price");
            auto quantity = boost::lexical_cast<double>(amount) / boost::lexical_cast<double>(exchFillPx);
            std::ostringstream exchQty;
            exchQty << quantity;
            std::cout <<"TRD: cid: " << clientOrderId << ", oid: " << exchID << ", fillID: " << fillID << ", fillQty: " << exchQty.str() 
                      << ", amt: " << exchFillPx << ", fillPx: " << exchFillPx << std::endl;
        } 
    } 
}

void ProcessTradeNotification(ptree const& info) {
    std::cout << __func__ << std::endl;
    auto params = info.get_child_optional("params");
    auto channel = params->get_optional<std::string>("channel");
    if (channel) std::cout << __func__ << ": Channel: " << *channel << std::endl;
    auto data = params->get_child_optional("data");
    if (data) {
        ptree::const_iterator end = data->end();
        for (ptree::const_iterator trdit = data->begin(); trdit != end; ++trdit) {
            std::cout << "TRD: ";
            std::cout << "order_id: " << trdit->second.get<std::string>("order_id") << ", ";
            std::cout << "label: " << trdit->second.get<std::string>("label") << ", ";
            std::cout << "price: " << trdit->second.get<std::string>("price") << ", ";
            std::cout << "amount: " << trdit->second.get<std::string>("amount") << ", ";
            std::cout << "fill_id: " << trdit->second.get<std::string>("trade_id") << ", ";
            std::cout << "liquidity: " << trdit->second.get<std::string>("liquidity") << ", ";
            std::cout << "direction: " << trdit->second.get<std::string>("direction") << "\n";

            auto exchID = trdit->second.get<std::string>("order_id");
            auto clientOrderId = trdit->second.get<std::string>("label");
            auto amount = trdit->second.get<std::string>("amount");
            auto fillID = trdit->second.get<std::string>("trade_id");
            bool maker = (trdit->second.get<std::string>("liquidity") == "M");
            auto exchFillPx = trdit->second.get<std::string>("price");
            auto quantity = boost::lexical_cast<double>(amount) / boost::lexical_cast<double>(exchFillPx);
            std::ostringstream exchQty;
            exchQty << quantity;
            std::cout <<"TRD: cid: " << clientOrderId << ", oid: " << exchID << ", fillID: " << fillID << ", fillQty: " << exchQty.str() 
                      << ", amt: " << exchFillPx << ", fillPx: " << exchFillPx << std::endl;
        } 
    } 
}

int main() {
    std::string msg = R"(
{"jsonrpc":"2.0","id":1,"result":{"trades":[{"trade_seq":57196858,"trade_id":"85012939","timestamp":1635427889618,"tick_direction":0,"state":"filled","self_trade":false,"reduce_only":false,"profit_loss":0.0,"price":61660.5,"post_only":false,"order_type":"limit","order_id":"6800409604","matching_id":null,"mark_price":61651.85,"liquidity":"T","label":"9upvib33","instrument_name":"BTC-PERPETUAL","index_price":61638.72,"fee_currency":"BTC","fee":0.00000551,"direction":"buy","amount":680.0}],"order":{"web":false,"time_in_force":"good_til_cancelled","replaced":false,"reduce_only":false,"profit_loss":0.0,"price":62579.5,"post_only":false,"order_type":"limit","order_state":"filled","order_id":"6800409604","max_show":680.0,"last_update_timestamp":1635427889618,"label":"9upvib33","is_liquidation":false,"instrument_name":"BTC-PERPETUAL","filled_amount":680.0,"direction":"buy","creation_timestamp":1635427889618,"commission":0.00000551,"average_price":61660.5,"api":true,"amount":680.0}},"usIn":1635427889617916,"usOut":1635427889619933,"usDiff":2017,"testnet":true}
)";

    ptree pt;
    std::istringstream is(msg);
    read_json(is, pt);
    std::cout << "MSG: " << msg << std::endl;
    PrintNodes(pt);
    ProcessTrade(pt);

    // Notification
    msg = R"(
{ "jsonrpc": "2.0", "method": "subscription", "params": { 
    "channel": "user.trades.any.any.raw",
    "data": [
      { "trade_seq": 57303018, "trade_id": "85180917", "timestamp": 1635662122251, "tick_direction": 0, "state": "filled", "self_trade": false, "reduce_only": false,
        "profit_loss": -0.00001785, "price": 61280, "post_only": false, "order_type": "limit", "order_id": "6815237676", "matching_id": null, "mark_price": 61269.22,
        "liquidity": "M", "label": "sell1234", "instrument_name": "BTC-PERPETUAL", "index_price": 61234.87, "fee_currency": "BTC", "fee": 0, "direction": "sell", "amount": 200
      }
    ],
    "label": "test_sub_trades"
  }
}
)";
    is.str(msg);
    read_json(is, pt);
    PrintNodes(pt);
    ProcessTradeNotification(pt);
}


