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

void HandleError(boost::property_tree::ptree const& info) {
    auto error = info.get_child_optional("error");
    if (!error) {
        std::cout << "error not found" << std::endl;
        return; 
    }
    std::stringstream ss;
    FormatNodes(*error, ss);
    std::cout << std::endl;
    std::cout << __func__ << ": " << ss.str() << std::endl;
    std::cout << std::endl;
}

// {"jsonrpc": "2.0", "error": {"message": "invalid_credentials", "code": 13004}, "testnet": true, "usIn": 1635032652027935, "usOut": 1635032652028014, "usDiff": 79}
// {"jsonrpc": "2.0", "id": 17, "error": {"message": "unauthorized", "code": 13009}, "usIn": 1635047305421510, "usOut": 1635047305421560, "usDiff": 50, "testnet": true}
int main() {
    std::string aMsg = R"(
{"jsonrpc":"2.0","id":2,"error":{"message":"Invalid params","data":{"reason":"must be a multiple of contract size","param":"amount"},"code":-32602},"usIn":1635306030129165,"usOut":1635306030129583,"usDiff":418,"testnet":true}
)"; 

    ptree info;
    std::istringstream is(aMsg);
    read_json(is, info);
    PrintNodes(info);
    HandleError(info);

    aMsg = R"(
{"jsonrpc": "2.0", "id": 17, "error": {"message": "unauthorized", "code": 13009}, "usIn": 1635047305421510, "usOut": 1635047305421560, "usDiff": 50, "testnet": true}
)";  
    is.str(aMsg);
    read_json(is, info);
    PrintNodes(info);
    HandleError(info);
}


