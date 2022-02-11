
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

using boost::property_tree::ptree;


void PrintNodes(boost::property_tree::ptree const& pt, std::string pad ="")
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << pad << it->first << ": " << it->second.get_value<std::string>() << std::endl;
        if (it->second.begin() != it->second.end()) 
             PrintNodes(it->second, pad+"\t");
    }   
    std::cout << pad << "Done." << std::endl;
}


int main() {
    std::string ss = R"({"jsonrpc":"2.0","method":"subscription","params":{"channel":"trades.future.BTC.raw","data":[{"trade_seq":56624,"trade_id":"184443790","timestamp":1634269556218,"tick_direction":2,"price":59215.5,"mark_price":59216.7,"instrument_name":"BTC-15OCT21","index_price":59211.34,"direction":"buy","amount":1000.0}]}})";

    ptree pt;
    std::istringstream is(ss);
    read_json(is, pt);
    PrintNodes(pt);
}


