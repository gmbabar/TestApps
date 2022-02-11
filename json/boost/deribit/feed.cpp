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

using PARAM_TYPE = boost::optional<boost::property_tree::basic_ptree<std::__1::basic_string<char>, std::__1::basic_string<char>> &>;
void ProcessTrade(PARAM_TYPE params) {
    auto data = params->get_child_optional("data");
    if (!data) {
        std::cout << "Data not found" << std::endl;
        return;
    }
    for(auto &it : *data) {
        std::cout << "Instrument: " << it.second.get<std::string>("instrument_name") << std::endl;
        std::cout << "Price: " << it.second.get<std::string>("price") << std::endl;
        std::cout << "Amount: " << it.second.get<std::string>("amount") << std::endl;
    }
}

void ProcessPriceLevel(boost::property_tree::ptree &plevel, std::string& price, std::string& amount, std::string &type) {
    int idx = 0;
    for(auto &it : plevel) {
        if (it.second.begin() == it.second.end()) {
            switch (idx) {
            case 2:
                amount = it.second.get_value<std::string>();
                break;
            case 1:
                price = it.second.get_value<std::string>();
                break;
            case 0:
                type = it.second.get_value<std::string>();
                break;
            default:
                break;
            }   
            ++idx;
        } else {
            std::cout << "UNEXPECTED DATA" << std::endl;
        }   
    }
}

void ProcessBook(PARAM_TYPE params) {
    auto data = params->get_child_optional("data");
    if (!data) {
        std::cout << "Data not found" << std::endl;
        return;
    }

    std::cout << "Instrument: " << data->get<std::string>("instrument_name") << std::endl;

    // R"("bids":[],"asks":[["new",57948.0,10000.0],["delete",57947.5,0.0]])";
    std::cout << "Processing Bids..." << std::endl;
    auto bids = data->get_child("bids");
    for(auto &bid : bids) {
        std::string amount, price, type;
        ProcessPriceLevel(bid.second, price, amount, type);
        std::cout << "Amount: " << amount << std::endl;
        std::cout << "Price: " << price << std::endl;
        std::cout << "Type: " << type << std::endl;
    }
    std::cout << "Processing Asks..." << std::endl;
    auto asks = data->get_child("asks");
    for(auto &ask : asks) {
        std::string amount, price, type;
        ProcessPriceLevel(ask.second, price, amount, type);
        std::cout << "Amount: " << amount << std::endl;
        std::cout << "Price: " << price << std::endl;
        std::cout << "Type: " << type << std::endl;
    }
}

int main() {
    std::string aMsg = R"({"jsonrpc":"2.0","method":"subscription",)" 
                     R"("params":{"channel":"trades.future.BTC.raw",)"
                     R"("data":[)"
                        R"({"trade_seq":56624,"trade_id":"184443790","timestamp":1634269556218,"tick_direction":2,"price":59215.5,"mark_price":59216.7,"instrument_name":"BTC-15OCT21","index_price":59211.34,"direction":"buy","amount":1000.0})"
                        R"(,{"trade_seq":56626,"trade_id":"184443791","timestamp":1634269556456,"tick_direction":2,"price":59216.6,"mark_price":59217.8,"instrument_name":"BTC-15OCT21","index_price":59211.34,"direction":"buy","amount":200.0})"
                     R"(]}})";

    aMsg = R"({"jsonrpc":"2.0","method":"subscription","params":{"channel":"book.BTC-31DEC21.raw",)"
           R"("data":{"type":"change","timestamp":1634094522978,"prev_change_id":35830365981,"instrument_name":"BTC-31DEC21","change_id":35830365982,)"
           R"("bids":[],"asks":[["new",57948.0,10000.0],["delete",57947.5,0.0]]}}})";
    std::cout << "MSG: " << aMsg << std::endl;
    ptree pt;
    std::istringstream is(aMsg);
    read_json(is, pt);
    PrintNodes(pt);

    auto params = pt.get_child_optional("params");
    if (!params) {
        std::cout << "Params not found" << std::endl;
        return 0;
    }
    auto channel = params->get<std::string>("channel");
    std::cout << "channel: " << params->get<std::string>("channel") << std::endl;
    if ( channel.compare( 0, 7, "trades." ) == 0 ) {
        ProcessTrade(params);
        return 0;
    } else if ( channel.compare( 0, 5, "book." ) == 0 ) {
        ProcessBook(params);
    }
}


