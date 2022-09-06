#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <map>

void ProcessBalance(boost::property_tree::ptree const& pTree, std::map<std::string, double> &balanceMap) {
    auto params = pTree.get_child_optional("params");
    if(!params) {
        std::cerr << "Params Not Found" << std::endl;
        return;
    }
    auto data = params->get_child_optional("data");
    if(!data) {
        std::cerr << "Data Not Found" << std::endl;
        return;
    }
    auto balance = data->get<double>("balance");
    auto ccy = data->get<std::string>("currency");
    balanceMap[ccy] = balance;
}

void PrintBalances(std::map<std::string, double> &balanceMap) {
    for(auto itr : balanceMap) {
        std::cout << itr.first << ":" << itr.second << std::endl;
    }
}

int main() {
    std::map<std::string, double> balanceMap;
    std::string jsonBTC = R"({"jsonrpc":"2.0","method":"subscription","params":{"channel":"user.portfolio.btc","data":{"total_pl":0,"session_upl":0,"session_rpl":0,"projected_maintenance_margin":0,"projected_initial_margin":0,"projected_delta_total":0,"portfolio_margining_enabled":false,"options_vega":0,"options_value":0,"options_theta":0,"options_session_upl":0,"options_session_rpl":0,"options_pl":0,"options_gamma":0,"options_delta":0,"margin_balance":799.56391616,"maintenance_margin":0,"initial_margin":0,"futures_session_upl":0,"futures_session_rpl":0,"futures_pl":0,"fee_balance":0,"estimated_liquidation_ratio_map":{},"estimated_liquidation_ratio":0,"equity":799.56391616,"delta_total_map":{},"delta_total":0,"currency":"BTC","balance":799.56391616,"available_withdrawal_funds":799.56391616,"available_funds":799.56391616}}})";
    std::string jsonETH = R"({"jsonrpc":"2.0","method":"subscription","params":{"channel":"user.portfolio.btc","data":{"total_pl":0,"session_upl":0,"session_rpl":0,"projected_maintenance_margin":0,"projected_initial_margin":0,"projected_delta_total":0,"portfolio_margining_enabled":false,"options_vega":0,"options_value":0,"options_theta":0,"options_session_upl":0,"options_session_rpl":0,"options_pl":0,"options_gamma":0,"options_delta":0,"margin_balance":799.56391616,"maintenance_margin":0,"initial_margin":0,"futures_session_upl":0,"futures_session_rpl":0,"futures_pl":0,"fee_balance":0,"estimated_liquidation_ratio_map":{},"estimated_liquidation_ratio":0,"equity":799.56391616,"delta_total_map":{},"delta_total":0,"currency":"ETH","balance":6.62208,"available_withdrawal_funds":799.56391616,"available_funds":6.62208}}})";
    std::string jsonSOL = R"({"jsonrpc":"2.0","method":"subscription","params":{"channel":"user.portfolio.btc","data":{"total_pl":0,"session_upl":0,"session_rpl":0,"projected_maintenance_margin":0,"projected_initial_margin":0,"projected_delta_total":0,"portfolio_margining_enabled":false,"options_vega":0,"options_value":0,"options_theta":0,"options_session_upl":0,"options_session_rpl":0,"options_pl":0,"options_gamma":0,"options_delta":0,"margin_balance":799.56391616,"maintenance_margin":0,"initial_margin":0,"futures_session_upl":0,"futures_session_rpl":0,"futures_pl":0,"fee_balance":0,"estimated_liquidation_ratio_map":{},"estimated_liquidation_ratio":0,"equity":799.56391616,"delta_total_map":{},"delta_total":0,"currency":"SOL","balance":0.00,"available_withdrawal_funds":799.56391616,"available_funds":0.00}}})";
    boost::property_tree::ptree pTreeBTC;
    boost::property_tree::ptree pTreeETH;
    boost::property_tree::ptree pTreeSOL;

    std::istringstream issBTC(jsonBTC);
    std::istringstream issETH(jsonETH);
    std::istringstream issSOL(jsonSOL);

    boost::property_tree::read_json(issBTC, pTreeBTC);
    boost::property_tree::read_json(issETH, pTreeETH);
    boost::property_tree::read_json(issSOL, pTreeSOL);

    ProcessBalance(pTreeBTC, balanceMap);
    ProcessBalance(pTreeETH, balanceMap);
    ProcessBalance(pTreeSOL, balanceMap);

    PrintBalances(balanceMap);

}