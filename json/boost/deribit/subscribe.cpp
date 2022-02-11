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
    std::cout << pad << "Done." << std::endl << std::endl;
}

void ProcessMsg(ptree &info) {
    auto msgId = info.get<unsigned>("id");
    std::cout << __func__ << ": ID: " << msgId << std::endl;

    auto result = info.get_child_optional("result");
    if (!result) {
        std::cout << "'result' not found" << std::endl;
        return;
    }

    //for (ptree::const_iterator it = result->begin(); it != result->end(); ++it) {
    //    if (it->second.begin() == it->second.end()) 
    for (auto &it : *result) {
        if (it.second.begin() == it.second.end()) 
            std::cout << __func__ << ": Channel: " << it.second.get_value<std::string>() << std::endl;
    }   
}

int main() {
    std::string aMsg = R"(
{
  "jsonrpc": "2.0", "id": 6,
  "result": ["user.trades.any.any.raw"],
  "usIn": 1635661842888548, "usOut": 1635661842888928, "usDiff": 380, "testnet": true
}
)"; 
    std::cout << "MSG: " << aMsg << std::endl;
    ptree pt;
    std::istringstream is(aMsg);
    read_json(is, pt);
    PrintNodes(pt);
    ProcessMsg(pt);
}


