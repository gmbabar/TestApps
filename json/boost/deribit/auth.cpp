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


int main() {
    std::string aMsg = R"(
 {
  "jsonrpc": "2.0",
  "id": 3,
  "result": {
    "token_type": "bearer",
    "scope": "account:read_write block_trade:read connection custody:read mainaccount name:TestFirst trade:read_write wallet:read",
    "refresh_token": "1666568507557.1D5P-_ZG.1bERM3ig9wQefPZZbnQGfoxqFo5T3jBSs7afPs4x4kn52dQMpsEcTGooY7oyz8bUbUkb41jN3atkqTdAq4-kaQxJsxG5AcvomMiC-8KkKAWsqkfgWBPrJevfAPrPvyM9qro9nDQ45Gp0cQR1bHse1L1kwITkUD78k83oCOGOgNG5f67aVJefzUPhSNeKwvl0yG8Q8pwb8Cx62RAbFsBwAHzrHMCXCqzcnc-qcSTTWQzMUUN2WlEUy1Sp1yBR7Uep3qS7zq5ZRS48MF1WCLzPrlp5yZp_MK2mJgx-RkchY5_dG-CbAcB2JERxh5SICjHamocuAVc",
    "expires_in": 31536000,
    "access_token": "1666568507557.1BMsOYvj.e8Wz_It11BLOiIRULexZAl19_f3QIbAiUzf5ChbjNd5J0QRDpLw5tBkpIQbr8yBKVqTzqf8llzkD3-So793EkkQJPwwuvwUfiwylgMac9ZZgb8d3d-RK0FUPgDyhQcKRmv1MT2UlVlOYnMp2Ku2CQCCrhslf7OtSoIfZFF3qnqg4LE8igocaMgUnyi3VIlqw2_7PqPqZmEvqT20fA70KQIMBk3fypIxyM_55NbkTyhKA_F8bgcB-vmA_YWyHS_4aNdo3S7uIf5KmeSLWKCOFr1W6KQNn3iAaWOfWB_oTr17o7NhqEHQ1eqxUoOVQxSt0hhNV8bN6"
  },
  "usIn": 1635032507557544,
  "usOut": 1635032507557846,
  "usDiff": 302,
  "testnet": true
 }
)"; 

    //std::cout << "MSG: " << aMsg << std::endl;
    ptree pt;
    std::istringstream is(aMsg);
    read_json(is, pt);
    PrintNodes(pt);

    auto result = pt.get_child_optional("result");
    if (!result) {
        std::cout << "result not found" << std::endl;
        return 0;
    }
    std::cout << "expires: " << result->get<unsigned long>("expires_in") << std::endl;
    std::cout << "token_type: " << result->get<std::string>("token_type") << std::endl;
}


