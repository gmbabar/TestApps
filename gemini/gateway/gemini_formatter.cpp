#include <bits/stdc++.h>
#include <boost/beast/http.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
using namespace std;
namespace nebula{
    namespace gw {
        struct ExchContract {
            std::string exchSym;
        };
        struct AuthInfo {
            std::string apiKey;
            std::string apiSecret;
        };
    }
    namespace tr{
        struct Order {
            std::string price;
            std::string size;
            std::string tif;
            std::string type;
            std::string side;
            std::string id;
            std::string options;
        };
    }
}


inline bool fmtGeminiSpotRestApiOrder(
        const std::string &aApiHost,
        const nebula::gw::AuthInfo &aAuthInfo,
        const nebula::gw::ExchContract &aExchContract,
        const nebula::tr::Order &aOrder,
        boost::beast::http::request<boost::beast::http::string_body> &aRequest,
        const char **anErrorPtr) {
            namespace beast = boost::beast;
            namespace http = beast::http;

        const auto symbol = aExchContract.exchSym;
        const auto size = aOrder.size;
        const auto price = aOrder.price;
        const auto tif = aOrder.tif;
        const auto side = aOrder.side;
        const auto target = "/v1/order/new";
        // const auto options = ''

        std::ostringstream oss;
        oss << "{"
            << R"("request":")" << target << '"'
            << R"(, "nonce":)" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
            << R"(, "client_order_id":")" << aOrder.id << '"'
            << R"(, "symbol":")" << symbol << '"'
            << R"(, "amount":")" << size << '"'
            << R"(, "price":")" << price << '"'
            << R"(, "side":")" << side << '"'
            << R"(, "type":")" << aOrder.type << '"'
            << R"(, "options":[")" << tif << "\"]"
            << "}";
    const auto payload = oss.str();
    std::cout << payload << std::endl;
    aRequest.set(http::field::host, aApiHost);
    aRequest.target(target);
    char buffer[1024];
    auto encSize = boost::beast::detail::base64::encode(buffer, payload.c_str(), payload.size());
    aRequest.set("X-GEMINI-PAYLOAD", std::string(buffer));
    // aRequest.body() = oss.str();
    return true;

}

inline bool fmtGeminiSpotRestApiCancelOrder(
        const std::string &aApiHost,
        const nebula::gw::AuthInfo &aAuthInfo,
        const nebula::gw::ExchContract &aExchContract,
        const nebula::tr::Order &aOrder,
        boost::beast::http::request<boost::beast::http::string_body> &aRequest,
        const char **anErrorPtr) {
            namespace beast = boost::beast;
            namespace http = beast::http;

        const auto target = "/v1/order/cancel";
        // const auto options = ''

        std::ostringstream oss;
        oss << "{"
            << R"("request":")" << target << '"'
            << R"(, "nonce":)" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
            << R"(, "order_id":")" << aOrder.id << '"'
            << "}";
    const auto payload = oss.str();
    std::cout << payload << std::endl;
    aRequest.set(http::field::host, aApiHost);
    aRequest.target(target);
    char buffer[1024];
    auto size = boost::beast::detail::base64::encode(buffer, payload.c_str(), payload.size());
    aRequest.set("X-GEMINI-PAYLOAD", std::string(buffer));
    // aRequest.body() = oss.str();
    return true;
}

inline bool fmtGeminiSpotRestApiActiveOrders(
        const std::string &aApiHost,
        const nebula::gw::AuthInfo &aAuthInfo,
        boost::beast::http::request<boost::beast::http::string_body> &aRequest,
        const char **anErrorPtr) {
            namespace beast = boost::beast;
            namespace http = beast::http;

        const auto target = "/v1/orders";
        // const auto options = ''

        std::ostringstream oss;
        oss << "{"
            << R"("request":")" << target << '"'
            << R"(, "nonce":)" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
            << "}";
    const auto payload = oss.str();
    std::cout << payload << std::endl;
    aRequest.set(http::field::host, aApiHost);
    aRequest.target(target);
    char buffer[1024];
    auto size = boost::beast::detail::base64::encode(buffer, payload.c_str(), payload.size());
    aRequest.set("X-GEMINI-PAYLOAD", std::string(buffer));
    // aRequest.body() = oss.str();
    return true;
}

inline bool fmtGeminiSpotRestApiBalances(
        const std::string &aApiHost,
        const nebula::gw::AuthInfo &aAuthInfo,
        boost::beast::http::request<boost::beast::http::string_body> &aRequest,
        const char **anErrorPtr) {
            namespace beast = boost::beast;
            namespace http = beast::http;

        const auto target = "/v1/balances";
        // const auto options = ''

        std::ostringstream oss;
        oss << "{"
            << R"("request":")" << target << '"'
            << R"(, "nonce":)" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
            << "}";
    const auto payload = oss.str();
    std::cout << payload << std::endl;
    aRequest.set(http::field::host, aApiHost);
    aRequest.target(target);
    char buffer[1024];
    auto size = boost::beast::detail::base64::encode(buffer, payload.c_str(), payload.size());
    aRequest.set("X-GEMINI-PAYLOAD", std::string(buffer));
    // aRequest.body() = oss.str();
    return true;
}

inline void parseNewOrder(boost::property_tree::ptree tree){
    const auto orderId = tree.get<std::string>("order_id");
    const auto symbol = tree.get<std::string>("symbol");
    const auto side = tree.get<std::string>("side");
    const auto type = tree.get<std::string>("type");
    const auto price = tree.get<std::string>("price");
    const auto amount = tree.get<std::string>("original_amount");
    const auto options = tree.get_child_optional("options");
    std::cout << "|" << orderId << "|" << symbol << "|" << side << "|" << type << "|" << price << "|" << amount << "|" << std::endl;
    for(auto item : options.get()) {
        std::cout << item.second.get_value<std::string>() << std::endl;
    }
}

/*
[{"order_id": "107421210","id": "107421210","symbol": "ethusd","exchange": "gemini","avg_execution_price": "0.00","side": "sell","type": "exchange limit","timestamp": "1547241628","timestampms": 1547241628042,"is_live": true,"is_cancelled": false,"is_hidden": false,"was_forced": false,"executed_amount": "0","remaining_amount": "1","options": [],"price": "125.51","original_amount": "1"},{"order_id": "107421205","id": "107421205","symbol": "ethusd","exchange": "gemini","avg_execution_price": "125.41","side": "buy","type": "exchange limit","timestamp": "1547241626","timestampms": 1547241626991,"is_live": true,"is_cancelled": false,"is_hidden": false,"was_forced": false,"executed_amount": "0.029147","remaining_amount": "0.970853","options": [],"price": "125.42","original_amount": "1"}]

*/

inline void parseActiveOrders(boost::property_tree::ptree &tree) {
    for (auto &item : tree) {
        const auto &order = item.second;
        parseNewOrder(order);
    }
}

/*
[
    {
        "type": "exchange",
        "currency": "BTC",
        "amount": "1154.62034001",
        "available": "1129.10517279",
        "availableForWithdrawal": "1129.10517279"
    },
    {
        "type": "exchange",
        "currency": "USD",
        "amount": "18722.79",
        "available": "14481.62",
        "availableForWithdrawal": "14481.62"
    },
    {
        "type": "exchange",
        "currency": "ETH",
        "amount": "20124.50369697",
        "available": "20124.50369697",
        "availableForWithdrawal": "20124.50369697"
    }
]
*/
inline void parseBalancesReport(boost::property_tree::ptree &tree) {
    for (auto &item : tree) {
        const auto &balance = item.second;
        const auto type = balance.get<std::string>("type");
        const auto ccy = balance.get<std::string>("currency");
        const auto qty = balance.get<std::string>("amount");
        std::cout << "|" << type << "|" << ccy << "|" << qty << "|" << std::endl;
    }
}



/*

{
    "order_id":"106817811",
    "id":"106817811",
    "symbol":"btcusd",
    "exchange":"gemini",
    "avg_execution_price":"3632.85101103",
    "side":"buy",
    "type":"exchange limit",
    "timestamp":"1547220404",
    "timestampms":1547220404836,
    "is_live":false,
    "is_cancelled":true,
    "is_hidden":false,
    "was_forced":false,
    "executed_amount":"3.7610296649",
    "remaining_amount":"1.2389703351",
    "reason":"Requested",
    "options":[],
    "price":"3633.00",
    "original_amount":"5"
}
*/
inline void parseCancelOrder(boost::property_tree::ptree &tree){
    const auto orderId = tree.get<std::string>("order_id");
    const auto symbol = tree.get<std::string>("symbol");
    const auto side = tree.get<std::string>("side");
    const auto type = tree.get<std::string>("type");
    const auto price = tree.get<std::string>("price");
    const auto amount = tree.get<std::string>("original_amount");
    const auto cancelled = tree.get<bool>("is_cancelled");
    const auto options = tree.get_child_optional("options");
    std::cout << "|" << orderId << "|" << symbol << "|" << side << "|" << type << "|" << price << "|" << amount << "|" << (cancelled==1 ? "true":"false") << "|" << std::endl;
    for(auto item : options.get()) {
        std::cout << item.second.get_value<std::string>() << std::endl;
    }
}
// {"order_id": "106817811","id": "106817811","symbol": "btcusd","exchange": "gemini","avg_execution_price": "3632.8508430064554","side": "buy","type": "exchange limit","timestamp": "1547220404","timestampms": 1547220404836,"is_live": true,"is_cancelled": false,"is_hidden": false,"was_forced": false,"executed_amount": "3.7567928949","remaining_amount": "1.2432071051","client_order_id": "20190110-4738721","options": ["maker-or-cancel"],"price": "3633.00","original_amount": "5"}
int main() {
    // nebula::gw::AuthInfo authInfo;
    // authInfo.apiKey = "account-8BDIOhP2VnBAzQHAn6DT";
    // authInfo.apiSecret = "mXob3f85YcHu2KEQDxXeVSMtabL";
    // nebula::gw::ExchContract exchContract;
    // exchContract.exchSym = "btcusd";
    // nebula::tr::Order order;
    // order.id = "12345678";
    // order.price = "9500";
    // order.size = "0.5";
    // order.side = "buy";
    // order.type = "exchange limit";
    // // order.tif = "GTC";
    // order.tif = "maker-or-cancel";
    // boost::beast::http::request<boost::beast::http::string_body> req_;
    // const char **error;
    // fmtGeminiSpotRestApiOrder("UNKNOWN", authInfo, exchContract, order, req_, error);
    // fmtGeminiSpotRestApiCancelOrder("UNKNOWN", authInfo, exchContract, order, req_, error);
    // fmtGeminiSpotRestApiBalances("UNKNOWN", authInfo, req_, error);
    // fmtGeminiSpotRestApiActiveOrders("UNKNOWN", authInfo, req_, error);
    std::string msg = R"({"order_id": "106817811","id": "106817811","symbol": "btcusd","exchange": "gemini","avg_execution_price": "3632.8508430064554","side": "buy","type": "exchange limit","timestamp": "1547220404","timestampms": 1547220404836,"is_live": true,"is_cancelled": false,"is_hidden": false,"was_forced": false,"executed_amount": "3.7567928949","remaining_amount": "1.2432071051","client_order_id": "20190110-4738721","options": ["maker-or-cancel"],"price": "3633.00","original_amount": "5"})";
    std::string balancesMsg = R"([{"type": "exchange","currency": "BTC","amount": "1154.62034001","available": "1129.10517279","availableForWithdrawal": "1129.10517279"},{"type": "exchange","currency": "USD","amount": "18722.79","available": "14481.62","availableForWithdrawal": "14481.62"},{"type": "exchange","currency": "ETH","amount": "20124.50369697","available": "20124.50369697","availableForWithdrawal": "20124.50369697"}])";
    std::string cancelMsg = R"({"order_id":"106817811","id":"106817811","symbol":"btcusd","exchange":"gemini","avg_execution_price":"3632.85101103","side":"buy","type":"exchange limit","timestamp":"1547220404","timestampms":1547220404836,"is_live":false,"is_cancelled":true,"is_hidden":false,"was_forced":false,"executed_amount":"3.7610296649","remaining_amount":"1.2389703351","reason":"Requested","options":[],"price":"3633.00","original_amount":"5"})";
    std::string activeOrders = R"([{"order_id": "107421210","id": "107421210","symbol": "ethusd","exchange": "gemini","avg_execution_price": "0.00","side": "sell","type": "exchange limit","timestamp": "1547241628","timestampms": 1547241628042,"is_live": true,"is_cancelled": false,"is_hidden": false,"was_forced": false,"executed_amount": "0","remaining_amount": "1","options": [],"price": "125.51","original_amount": "1"},{"order_id": "107421205","id": "107421205","symbol": "ethusd","exchange": "gemini","avg_execution_price": "125.41","side": "buy","type": "exchange limit","timestamp": "1547241626","timestampms": 1547241626991,"is_live": true,"is_cancelled": false,"is_hidden": false,"was_forced": false,"executed_amount": "0.029147","remaining_amount": "0.970853","options": [],"price": "125.42","original_amount": "1"}])";
    std::stringstream ss(msg);
    boost::property_tree::ptree info;
    boost::property_tree::read_json(ss, info);
    parseNewOrder(info);
    std::cout << "-----------------------------------" << std::endl;
    ss.str("");
    ss << balancesMsg;
    info.clear();
    boost::property_tree::read_json(ss, info);
    parseBalancesReport(info);
    std::cout << "-----------------------------------" << std::endl;
    ss.str("");
    ss << cancelMsg;
    info.clear();
    boost::property_tree::read_json(ss, info);
    parseCancelOrder(info);
    std::cout << "-----------------------------------" << std::endl;
    ss.str("");
    ss << activeOrders;
    info.clear();
    boost::property_tree::read_json(ss, info);
    parseActiveOrders(info);
    std::cout << "-----------------------------------" << std::endl;
}