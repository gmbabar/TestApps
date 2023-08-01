#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;

void ParseAccountBalance(const std::string& response)
{
    ptree pt;

    try {
        std::istringstream iss(response);
        read_json(iss, pt);
    } catch (const boost::property_tree::json_parser::json_parser_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }

    const auto &error = pt.get_child_optional("error");
    if (!error || error->size() > 0) {
        std::cerr << "Balance API error: " << std::endl;
        for (const auto &err : *error)
            std::cerr << "\terror: " << err.second.get_value<std::string>() << std::endl;
        return;
    }

    const auto &results = pt.get_child_optional("result");
    if (!results) {
        std::cerr << "Unexpected Json Msg Received: " << response << std::endl;
        return;
    }
    for (const auto& balance : *results) {
        std::string currency = balance.first;
        const auto balanceData = balance.second.get_child_optional("balance");
        if (!balanceData)
            continue;
        std::cout << "Currency: " << currency << ", Balance: " << balanceData->get_value<std::string>() << std::endl;
    }
}

void ParseOpenOrders(const std::string& response)
{
    ptree pt;

    try {
        std::istringstream iss(response);
        read_json(iss, pt);
    } catch (const boost::property_tree::json_parser::json_parser_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }

    auto error = pt.get_child("error");
    if (error.size() > 0) {
        std::string error = pt.get<std::string>("error");
        std::cerr << "API error: " << error << std::endl;
        return;
    }

    ptree orders = pt.get_child("result.open");
    for (const auto& order : orders) {
        std::string orderID = order.first;
        ptree orderData = order.second;

        std::string symbol = orderData.get<std::string>("descr.pair");
        std::string type = orderData.get<std::string>("descr.type");
        std::string status = orderData.get<std::string>("status");
        double price = orderData.get<double>("descr.price");
        double volume = orderData.get<double>("vol");

        std::cout << "Order ID: " << orderID << ", Symbol: " << symbol << ", Type: " << type
                  << ", Status: " << status << ", Price: " << price << ", Volume: " << volume << std::endl;
    }
}

void ParseOrderResponse(const std::string& response)
{
    ptree pt;

    try {
        std::istringstream iss(response);
        read_json(iss, pt);
    } catch (const boost::property_tree::json_parser::json_parser_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }

    auto error = pt.get_child("error");
    if (error.size() > 0) {
        std::string error = pt.get<std::string>("error");
        std::cerr << "API error: " << error << std::endl;
        return;
    }

    const auto &result = pt.get_child("result");
    std::string order = result.get<std::string>("descr.order");
    // std::string close = result.get<std::string>("descr.close");      // optional
    const auto txid = result.get_child("txid");
    for(const auto &id : txid) {
        std::cout << "Transaction ID: " << id.second.get_value<std::string>() << std::endl;
    }

    std::cout << "Order: " << order << std::endl;
    // std::cout << "Close: " << close << std::endl;        // optional
}

void ParseCancelOrderResponse(const std::string& response)
{
    ptree pt;

    try {
        std::istringstream iss(response);
        read_json(iss, pt);
    } catch (const boost::property_tree::json_parser::json_parser_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }

    auto error = pt.get_child("error");
    if (error.size() > 0) {
        std::string error = pt.get<std::string>("error");
        std::cerr << "API error: " << error << std::endl;
        return;
    }

    const auto count = pt.get<int>("result.count");
    std::cout << "Cancelled Orders: " << count << std::endl;
}

void ParseWebAuthResponse(const std::string& response)
{
    ptree pt;

    try {
        std::istringstream iss(response);
        read_json(iss, pt);
    } catch (const boost::property_tree::json_parser::json_parser_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }

    auto error = pt.get_child("error");
    if (error.size() > 0) {
        std::string error = pt.get<std::string>("error");
        std::cerr << "API error: " << error << std::endl;
        return;
    }

    const auto token = pt.get<std::string>("result.token");
    std::cout << "Authentication Token: " << token << std::endl;
}

void ParseEditOrderResponse(const std::string& jsonString)
{
    ptree pt;

    try {
        std::istringstream iss(jsonString);
        read_json(iss, pt);
    } catch (const boost::property_tree::json_parser::json_parser_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }

    try {
        std::string status = pt.get<std::string>("result.status");
        std::string txid = pt.get<std::string>("result.txid");
        std::string originaltxid = pt.get<std::string>("result.originaltxid");
        std::string volume = pt.get<std::string>("result.volume");
        double price = pt.get<double>("result.price");
        double price2 = pt.get<double>("result.price2");
        int ordersCancelled = pt.get<int>("result.orders_cancelled");
        std::string order = pt.get<std::string>("result.descr.order");

        std::cout << "Status: " << status << std::endl;
        std::cout << "Txid: " << txid << std::endl;
        std::cout << "Original Txid: " << originaltxid << std::endl;
        std::cout << "Volume: " << volume << std::endl;
        std::cout << "Price: " << price << std::endl;
        std::cout << "Price2: " << price2 << std::endl;
        std::cout << "Orders Cancelled: " << ordersCancelled << std::endl;
        std::cout << "Order: " << order << std::endl;
    } catch (const boost::property_tree::ptree_error& e) {
        std::cerr << "Error accessing order response: " << e.what() << std::endl;
    }
}

int main()
{
    // std::string balResponse = R"({"error": [],"result": {"ZUSD": "100.0","XXBT": "2.5","ETH": "10.0"}})";
    // std::string openResponse = R"({"error":[],"result":{"open":{"OLG43K-4XBY7-DC7XW3":{"refid":null,"userref":0,"status":"open","opentm":1650902321.9519,"starttm":0,"expiretm":0,"descr":{"pair":"XBTUSD","type":"buy","ordertype":"limit","price":"30000.0","price2":"0.0","leverage":"none","order":"buy 0.01 XBTUSD @ limit 30000.0"},"vol":"0.01","vol_exec":"0.0","cost":"0.0","fee":"0.0","price":"0.0","stopprice":"0.0","limitprice":"0.0","misc":"","oflags":"","trades":[]},"O7E2C6-7BUX3-N4X9VC":{"refid":null,"userref":0,"status":"open","opentm":1650902324.2887,"starttm":0,"expiretm":0,"descr":{"pair":"ETHUSD","type":"sell","ordertype":"limit","price":"2000.0","price2":"0.0","leverage":"none","order":"sell 0.5 ETHUSD @ limit 2000.0"},"vol":"0.5","vol_exec":"0.0","cost":"0.0","fee":"0.0","price":"0.0","stopprice":"0.0","limitprice":"0.0","misc":"","oflags":"","trades":[]}}}})";
    // std::string newOrderResponse = R"({"error": [],"result": {"descr": {"order": "buy 2.12340000 XBTUSD @ limit 45000.1 with 2:1 leverage","close": "close position @ stop loss 38000.0 -> limit 36000.0"},"txid": ["OUF4EM-FRGI2-MQMWZD"]}})";
    // std::string cancelOrderResponse = R"({"error": [],"result": {"count": 4}})";
    // std::string webAuth = R"({"error": [],"result": {"token": "1Dwc4lzSwNWOAwkMdqhssNNFhs1ed606d1WcF3XfEMw","expires": 900}})";
    std::string editOrder = R"({"error": [],"result": {"status": "ok","txid": "OFVXHJ-KPQ3B-VS7ELA","originaltxid": "OHYO67-6LP66-HMQ437","volume": "0.00030000","price": "19500.0","price2": "32500.0","orders_cancelled": 1,"descr": {"order": "buy 0.00030000 XXBTZGBP @ limit 19500.0"}}})";
    // ParseCancelOrderResponse(cancelOrderResponse);
    // ParseOpenOrders(response);
    // ParseAccountBalance(response);
    // ParseWebAuthResponse(webAuth);
    ParseEditOrderResponse(editOrder);


    return 0;
}


