#include <iostream>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "bufrange.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <vector>

using namespace rapidjson;



namespace SlowParser {
    /*
    -----------Top-Of-Book

    {"ch":"orderbook/top/100ms","data":{"BTCUSDT":{"t":1669908019703,"a":"16984.70","A":"0.00352","b":"16977.15","B":"0.01041"}}}

    */

    inline void parseTopOfBook(rapidjson::Document &doc) {
        std::string askPX, bidPX, askQty, bidQty;
        auto json = doc["data"].GetObject();
        std::cout << __func__ << std::endl;
        std::string symbol = json.MemberBegin()->name.GetString();
        auto innerData = json[symbol.c_str()].GetObject();
        askPX = innerData["a"].GetString();
        askQty = innerData["A"].GetString();
        bidPX = innerData["b"].GetString();
        bidQty = innerData["B"].GetString();

        std::cout << askPX << " | " << askQty << " | " << bidPX << " | " << bidQty << std::endl;
    }

    /*
    ----------Trade-Update
    {"ch":"trades","update":{"BTCUSDT":[{"t":1661352113934,"i":1872478761,"p":"21460.55","q":"0.00105","s":"buy"},{"t":1661352113934,"i":1872478762,"p":"21460.56","q":"0.07000","s":"buy"}]}}
    */

    inline void parseTrade(rapidjson::Document &document) {
        // Document document;
        // document.Parse(json.c_str());
        std::cout << "channel: " << document["trades"].GetString() << std::endl;
        auto obj = document["update"].IsObject() ? document["update"].GetObject() : document["snapshot"].GetObject();
        std::cout << "symbol: " << obj["symbol"].GetString() << std::endl;
        const char* symbol = obj["symbol"].GetString();
        auto var = document[symbol].GetArray();
        document["update"].IsObject() ? std::cout << "update : [" << std::endl : std::cout << "snapshot : [" << std::endl;
        // std::cout << "update : [" << std::endl;
        for (SizeType i = 0; i < var.Size(); i++) {
            auto arrVal = var[i].GetObject();
            std::cout << "time_ms: " << arrVal["t"].GetInt64() << std::endl;
            std::cout << "Trade identifier: " << arrVal["i"].GetInt64() << std::endl;
            std::cout << "price: " << arrVal["p"].GetString() << std::endl;
            std::cout << "quantity: " << arrVal["q"].GetString() << std::endl;
            std::cout << "side: " << arrVal["s"].GetString() << std::endl;
        }
        std::cout << "]" << std::endl;
    }

    // {"ch": "trades","snapshot": {"BTCUSDT": [{"t": 1626861109494,"i": 1555634969,"p": "30881.96","q": "12.66828","s": "buy"}]}}

    /*
    ----------Trade-Snapshot
    */

    inline void parseTradeSnapshot(rapidjson::Document &doc) {
        std::string side, px, qty, symbol;
        symbol = doc.MemberBegin()->name.GetString();
        auto innerArr = doc[symbol.c_str()].GetArray();
        for(auto idx=0; innerArr.Size(); idx++) {
            auto data = innerArr[idx].GetObject();
            px = data["p"].GetString();
            qty = data["q"].GetString();
            side = data["s"].GetString();
            std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << px << ", " << qty << ", " << side << std::endl;
        }
    }


    // {"ch":"ticker/1s","data":{"BTCUSDT":{"t":1670238331387,"a":"17290.25","A":"0.10000","b":"17280.80","B":"0.07226","o":"16997.28","c":"17285.54","h":"17413.05","l":"16905.89","v":"19842.66231","q":"340540322.4741842","p":"288.26","P":"1.695918405768452","L":1998467391}}}

    void parseTicker(rapidjson::Document &doc) {
        std::string askPX, bidPX, askQty, bidQty;
        auto json = doc["data"].GetObject();
        std::cout << __func__ << std::endl;
        std::string symbol = json.MemberBegin()->name.GetString();
        auto innerData = json[symbol.c_str()].GetObject();
        askPX = innerData["a"].GetString();
        askQty = innerData["A"].GetString();
        bidPX = innerData["b"].GetString();
        bidQty = innerData["B"].GetString();

        std::cout << askPX << " | " << askQty << " | " << bidPX << " | " << bidQty << std::endl;
    }
}

namespace FastParser {
    /*
    -----------Top-Of-Book

    {"ch":"orderbook/top/100ms","data":{"BTCUSDT":{"t":1669908019703,"a":"16984.70","A":"0.00352","b":"16977.15","B":"0.01041"}}}

    */

    inline void parseTopOfBook(std::string &json) {
        const char *aBuf = json.c_str();
        auto sPos = json.find("data");
        auto col = json.find(':', sPos);
        auto start = json.find('{', col);
        auto symS = json.find('"', start+1);
        auto symE = json.find('"', symS+1);
        Parse::data::ConstBufRange symbol(&aBuf[symS]+1, &aBuf[symE]);
        // auto sym = json.substr(symS+1, symE-symS-1);

        auto dataS = json.find('{', symE+1);
        auto askPxS = json.find("a", dataS);
        auto askPxC = json.find(':', askPxS+1);
        askPxS = json.find('"', askPxC);
        auto askPxE = json.find('"', askPxS+1);
        Parse::data::ConstBufRange askPx(&aBuf[askPxS]+1, &aBuf[askPxE]);
        // auto askPx = json.substr(askPxS+1, askPxE-askPxS-1);

        auto askQtyCol = json.find(':', askPxE);
        auto askQtyS = json.find('"', askQtyCol);
        auto askQtyE = json.find('"', askQtyS+1);

        Parse::data::ConstBufRange askQty(&aBuf[askQtyS]+1, &aBuf[askQtyE]);
        // auto askQty = json.substr(askQtyS+1, askQtyE-askQtyS-1);

        auto bidPxCol = json.find(':', askQtyE);
        auto bidPxS = json.find('"', bidPxCol+1);
        auto bidPxE = json.find('"', bidPxS+1);
        Parse::data::ConstBufRange bidPx(&aBuf[bidPxS]+1, &aBuf[bidPxE]);
        // auto bidPx = json.substr(bidPxS+1, bidPxE-bidPxS-1);

        auto bidQtyCol = json.find(':', bidPxE);
        auto bidQtyS = json.find('"', bidQtyCol+1);
        auto bidQtyE = json.find('"', bidQtyS+1);
        Parse::data::ConstBufRange bidQty(&aBuf[bidQtyS]+1, &aBuf[bidQtyE]);
        // auto bidQty = json.substr(bidQtyS+1, bidQtyE-bidQtyS-1);
        // std::cout << askPx << " | " << askQty << " | " << bidPx << " | " << bidQty << std::endl;
        std::cout << askPx.asStr() << " | " << askQty.asStr() << " | " << bidPx.asStr() << " | " << bidQty.asStr() << std::endl;
    }


    // {"ch":"ticker/1s","data":{"BTCUSDT":{"t":1670238331387,"a":"17290.25","A":"0.10000","b":"17280.80","B":"0.07226","o":"16997.28","c":"17285.54","h":"17413.05","l":"16905.89","v":"19842.66231","q":"340540322.4741842","p":"288.26","P":"1.695918405768452","L":1998467391}}}

    void parseTicker(std::string &json) {
        auto sPos = json.find("data");
        auto col = json.find(':', sPos);
        auto start = json.find('{', col);
        auto symS = json.find('"', start+1);
        auto symE = json.find('"', symS+1);
        const char *aBuf = json.c_str();

        Parse::data::ConstBufRange symbol(&aBuf[symS]+1, &aBuf[symE]);
        // auto sym = json.substr(symS+1, symE-symS-1);

        auto dataS = json.find('{', symE+1);
        auto askPxS = json.find("a", dataS);
        auto askPxC = json.find(':', askPxS+1);
        askPxS = json.find('"', askPxC);
        auto askPxE = json.find('"', askPxS+1);
        Parse::data::ConstBufRange askPx(&aBuf[askPxS]+1, &aBuf[askPxE]);

        // auto askPx = json.substr(askPxS+1, askPxE-askPxS-1);

        auto askQtyCol = json.find(':', askPxE);
        auto askQtyS = json.find('"', askQtyCol);
        auto askQtyE = json.find('"', askQtyS+1);
        Parse::data::ConstBufRange askQty(&aBuf[askQtyS]+1, &aBuf[askQtyE]);

        // auto askQty = json.substr(askQtyS+1, askQtyE-askQtyS-1);

        auto bidPxCol = json.find(':', askQtyE);
        auto bidPxS = json.find('"', bidPxCol+1);
        auto bidPxE = json.find('"', bidPxS+1);
        Parse::data::ConstBufRange bidPx(&aBuf[bidPxS]+1, &aBuf[bidPxE]);

        // auto bidPx = json.substr(bidPxS+1, bidPxE-bidPxS-1);

        auto bidQtyCol = json.find(':', bidPxE);
        auto bidQtyS = json.find('"', bidQtyCol+1);
        auto bidQtyE = json.find('"', bidQtyS+1);
        Parse::data::ConstBufRange bidQty(&aBuf[bidQtyS]+1, &aBuf[bidQtyE]);

        // auto bidQty = json.substr(bidQtyS+1, bidQtyE-bidQtyS-1);
        // std::cout << askPx << " | " << askQty << " | " << bidPx << " | " << bidQty << std::endl;
        std::cout << askPx.asStr() << " | " << askQty.asStr() << " | " << bidPx.asStr() << " | " << bidQty.asStr() << std::endl;
    }

    /*
    ----------Trade-Update
    {"ch":"trades","update":{"BTCUSDT":[{"t":1661352113934,"i":1872478761,"p":"21460.55","q":"0.00105","s":"buy"},{"t":1661352113934,"i":1872478762,"p":"21460.56","q":"0.07000","s":"buy"}]}}
    */

    inline void parseTradeUPD(std::string &json) {
        Parse::data::ConstBufRange side, px, qty;
        // std::string side, px, qty, symbol;
        const char *aBuf = json.c_str();
        int dataS, dataE;
        auto sPos = json.find("update");
        auto col = json.find(':', sPos);
        auto start = json.find('{', col);
        auto symS = json.find('"', start+1);
        auto symE = json.find('"', symS+1);
        Parse::data::ConstBufRange symbol(&aBuf[symS+1], &aBuf[symE]);
        // symbol = json.substr(symS+1, symE-symS-1);
        auto arrS = json.find('[', symE);
        auto arrE = json.find(']', arrS);

        auto arr = json.substr(arrS+1, arrE-arrS-1);
        aBuf = arr.c_str();
        int valStart, valEnd;
        // std::cout << arr << std::endl;
        for(int i=0; i<arr.size(); i++) {

            if(arr[i] == 'p') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                px.buf = &aBuf[valStart+1];
                px.bufEnd = &aBuf[valEnd];
                // px = arr.substr(valStart+1, valEnd-valStart-1);
            }
            if(arr[i] == 'q') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                qty.buf = &aBuf[valStart+1];
                qty.bufEnd = &aBuf[valEnd];
                // qty = arr.substr(valStart+1, valEnd-valStart-1);
            }
            if(arr[i] == 's') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                // side = arr.substr(valStart+1, valEnd-valStart-1);
                side.buf = &aBuf[valStart+1];
                side.bufEnd = &aBuf[valEnd];
                std::cout << "|" << side << " for " << px << "@" << qty << " on " << symbol << "|" << std::endl;
                // std::cout << symbol.asStr() << std::endl;
                if(arr.find("s", i+1) == std::string::npos) {
                    break;
                }
            }
        }
    }


    // {"ch": "trades","snapshot": {"BTCUSDT": [{"t": 1626861109494,"i": 1555634969,"p": "30881.96","q": "12.66828","s": "buy"}]}}

    /*
    ----------Trade-Snapshot
    */

    inline void parseTradeSnapshot(std::string &json) {
        Parse::data::ConstBufRange side, px, qty;
        // std::string side, px, qty, symbol;
        const char *aBuf = json.c_str();
        int dataS, dataE;
        auto sPos = json.find("snapshot");
        auto col = json.find(':', sPos);
        auto start = json.find('{', col);
        auto symS = json.find('"', start+1);
        auto symE = json.find('"', symS+1);
        // symbol = json.substr(symS+1, symE-symS-1);
        Parse::data::ConstBufRange symbol(&aBuf[symS+1], &aBuf[symE]);
        auto arrS = json.find('[', symE);
        auto arrE = json.find(']', arrS);

        auto arr = json.substr(arrS+1, arrE-arrS-1);
        aBuf = arr.c_str();
        int valStart, valEnd;
        // std::cout << arr << std::endl;
        for(int i=0; i<arr.size(); i++) {

            if(arr[i] == 'p') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                // px = arr.substr(valStart+1, valEnd-valStart-1);
                px.buf = &aBuf[valStart+1];
                px.bufEnd = &aBuf[valEnd];
            }
            if(arr[i] == 'q') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                // qty = arr.substr(valStart+1, valEnd-valStart-1);
                qty.buf = &aBuf[valStart+1];
                qty.bufEnd = &aBuf[valEnd];
            }
            if(arr[i] == 's') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                // side = arr.substr(valStart+1, valEnd-valStart-1);
                side.buf = &aBuf[valStart+1];
                side.bufEnd = &aBuf[valEnd];
                std::cout << "|" << side << " for " << px << "@" << qty << " on " << symbol << "|" << std::endl;
                if(arr.find("s", i+1) == std::string::npos) {
                    break;
                }
            }
        }

        // symbol = doc.MemberBegin()->name.GetString();
        // auto innerArr = doc[symbol.c_str()].GetArray();
        // for(auto idx=0; innerArr.Size(); idx++) {
        //     auto data = innerArr[idx].GetObject();
        //     px = data["p"].GetString();
        //     qty = data["q"].GetString();
        //     side = data["s"].GetString();
        //     std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << px << ", " << qty << ", " << side << std::endl;
        // }
    }
}

namespace FormattersPF {

/*
    {"method": "subscribe","ch": "ticker/1s","params": {"symbols": ["ETHBTC","BTCUSDT"]},"id": 123}
*/

    void formatSubscribeTicker(std::ostringstream &oss, std::vector<std::string> &symbols, std::string &id) {
        oss << "{"
            << "\"method\":\"subscribe\""
            << ",\"ch\":\"ticker/1s\""
            << ",\"params\":{\"symbols\":[";
            for(auto &ele : symbols) {
                oss << '"' << ele << '"' << ",";
            }
            oss << "\b]},";
            oss << "\"id\":" << id
            << "}";
        std::cout << oss.str() << std::endl;
    }

/*
    {"method": "subscribe","ch": "trades","params": {"symbols": ["ETHBTC", "BTCUSDT"],"limit": 1},"id": 123}
*/

    void formatSubscribeTrade(std::ostringstream &oss, std::vector<std::string> &symbols, std::string &id) {
        oss << "{"
            << "\"method\":\"subscribe\""
            << ",\"ch\":\"trades\""
            << ",\"params\":{\"symbols\":[";
            for(auto &ele : symbols) {
                oss << '"' << ele << '"' << ",";
            }
            oss << "\b]},";
            oss << "\"id\":" << id
            << "}";
        std::cout << oss.str() << std::endl;
    }

/*
{"method": "subscribe","ch": "orderbook/top/1000ms","params": {"symbols": ["ETHBTC", "BTCUSDT"]},"id": 123}
*/

    void formatSubscribeTopOfBook(std::ostringstream &oss, std::vector<std::string> &symbols, std::string &id, std::string &bookInterval) {
        oss << "{"
            << "\"method\":\"subscribe\""
            << ",\"ch\":\"orderbook/top/" << bookInterval << "\""
            << ",\"params\":{\"symbols\":[";
            for(auto &ele : symbols) {
                oss << '"' << ele << '"' << ",";
            }
            oss << "\b]},";
            oss << "\"id\":" << id
            << "}";
        std::cout << oss.str() << std::endl;
    }
}


namespace SpotGatewayParsers {

    /*
        {"jsonrpc":"2.0","result":true}
    */
    void parseLogin(boost::property_tree::ptree &jsonTree){
        auto result = jsonTree.get<bool>("result");
        if(result) {
            std::cout << "LOGIN SUCCESSFULL" << std::endl;
        }
        else {
            std::cout << "LOGIN FAILED" << std::endl;
        }
    }

    /*
        {"jsonrpc":"2.0","method":"spot_balance","params":[{"currency":"USDT","available":"1999.999999799978","reserved":"0.000000200022","reserved_margin":"0"}]}
    */
    void parseBalance(boost::property_tree::ptree &jsonTree) {
        std::string ccy, balance;
        auto arr = jsonTree.get_child("params");
        for(auto &obj : arr) {
            ccy = obj.second.get<std::string>("currency");
            balance = obj.second.get<std::string>("available");
        }
    }

/*
    {
        "jsonrpc":"2.0",
        "result":{
            "id":19584723200,
            "client_order_id":"harry222",
            "symbol":"BTCUSDT",
            "side":"buy",
            "status":"new",
            "type":"limit",
            "time_in_force":"GTC",
            "quantity":"0.00150",
            "quantity_cumulative":"0",
            "price":"9500.00",
            "post_only":false,
            "created_at":"2022-12-27T10:35:52.737Z",
            "updated_at":"2022-12-27T10:35:52.737Z",
            "report_type":"new"
        },
        "id":123
    }
*/

    void parseNewOrderResponse(boost::property_tree::ptree &jsonTree) {
        auto result = jsonTree.get_child("result");
        result.get<std::string>("client_order_id");
        result.get<std::string>("symbol");
        result.get<std::string>("side");
        result.get<std::string>("status");
        result.get<std::string>("time_in_force");
        result.get<std::string>("quantity");
        result.get<std::string>("quantity_cumulative");
        result.get<std::string>("price");
    }

/*

    {
        "jsonrpc":"2.0",
        "result":{
            "id":19584723200,
            "client_order_id":"harry222",
            "symbol":"BTCUSDT",
            "side":"buy",
            "status":"canceled",
            "type":"limit",
            "time_in_force":"GTC",
            "quantity":"0.00150",
            "quantity_cumulative":"0",
            "price":"9500.00",
            "post_only":false,
            "created_at":"2022-12-27T10:35:52.737Z",
            "updated_at":"2022-12-27T10:36:10.132Z",
            "report_type":"canceled"
        },
        "id":123
    }


*/
    void parseCancelOrderResponse(boost::property_tree::ptree &jsonTree) {
        auto result = jsonTree.get_child("result");
        result.get<std::string>("client_order_id");
        result.get<std::string>("symbol");
        result.get<std::string>("side");
        result.get<std::string>("status");
        result.get<std::string>("time_in_force");
        result.get<std::string>("quantity");
        result.get<std::string>("quantity_cumulative");
        result.get<std::string>("price");
    }

/*

{
    "jsonrpc":"2.0",
    "result": {
        "id":19588546870,
        "client_order_id":"harry223",
        "symbol":"BTCUSDT",
        "side":"buy",
        "status":"new",
        "type":"limit",
        "time_in_force":"GTC",
        "quantity":"0.00001",
        "quantity_cumulative":"0",
        "price":"0.02",
        "post_only":false,
        "created_at":"2022-12-28T07:54:07.961Z",
        "updated_at":"2022-12-28T07:54:30.174Z",
        "report_type":"replaced",
        "original_client_order_id":"harry222"
    },
    "id":123
}

*/

    void parseReplaceResponse(boost::property_tree::ptree &jsonTree) {
        auto result = jsonTree.get_child("result");
        result.get<std::string>("client_order_id");
        result.get<std::string>("symbol");
        result.get<std::string>("side");
        result.get<std::string>("status");
        result.get<std::string>("type");
        result.get<std::string>("time_in_force");
        result.get<std::string>("quantity");
        result.get<std::string>("quantity_cumulative");
        result.get<std::string>("price");
        result.get<std::string>("report_type");
        result.get<std::string>("original_client_order_id");
    }


/*

{
    "jsonrpc":"2.0",
    "method":"spot_order",
    "params":{
        "id":19592625059,
        "client_order_id":"harry225",
        "symbol":"BTCUSDT",
        "side":"buy",
        "status":"filled",
        "type":"limit",
        "time_in_force":"GTC",
        "quantity":"0.00150",
        "quantity_cumulative":"0.00150",
        "price":"16731.00",
        "post_only":false,
        "created_at":"2022-12-29T10:59:40.65Z",
        "updated_at":"2022-12-29T10:59:40.65Z",
        "report_type":"trade",
        "trade_id":1068784,
        "trade_quantity":"0.00150",
        "trade_price":"16724.16",
        "trade_fee":"0.002704296672",
        "trade_taker":true
    }
}

*/

    void parseExecutionReport(boost::property_tree::ptree &jsonTree) {
        auto result = jsonTree.get_child("params");
        result.get<std::string>("client_order_id");
        result.get<std::string>("symbol");
        result.get<std::string>("side");
        result.get<std::string>("status");
        result.get<std::string>("time_in_force");
        result.get<std::string>("quantity");
        result.get<std::string>("quantity_cumulative");
        result.get<std::string>("price");
        result.get<bool>("trade_taker");
    }

}

namespace formattersSpotGateway {
    void formatLogin(
        std::ostringstream &oss,
        std::string &aPriKey,
        std::string &aPubKey ) {

        oss << "{"
        << R"("method": "login")"
        << R"(,"params": {)"
            << R"("type": "BASIC")"
            << R"(,"api_key": ")" << aPriKey << '"'
            << R"(,"secret_key": ")" << aPubKey << '"'
        << R"(}})";
    }

    void formatNewOrder(
        std::ostringstream &oss,
        std::string &cliOrdId, std::string &symbol,
        std::string &side, std::string &type,
        std::string &quantity, std::string &price,
        std::string &tif, int id) {

        oss << R"({)"
            << R"("method":"spot_new_order")"
            << R"(,"params":{)"
            << R"("client_order_id":")" << cliOrdId << '"'
            << R"(,"symbol":")" << symbol << '"'
            << R"(,"side":")" << side << '"'
            << R"(,"type":)" << type << '"'
            << R"(,"time_in_force":")" << tif << '"'
            << R"(,"quantity":")" << quantity << '"'
            << R"(,"price":")" << price << '"'
            << R"(},)"
            << R"("id":)" << id << '}';
    }

    void formatCancelOrder(
        std::ostringstream &oss,
        std::string &cliOrdId,
        int id) {

        oss << R"({)"
            << R"("method":"spot_cancel_order")"
            << R"(,"params":{)"
            << R"("client_order_id":")" << cliOrdId << '"'
            << R"(},)"
            << R"("id":)" << id
            << R"(})";
    }

    void formatBalanceSubscribe(std::ostringstream &oss, int id) {
        oss << R"({)"
            << R"("method": "spot_balance_subscribe","params": {"mode": "batches"},)"
            << R"("id":)" << id << '}';
    }

    void formatSubscribe(std::ostringstream &oss, int id) {
        oss << "{"
            << R"("method":"spot_subscribe")"
            << R"(,"params":{})"
            << R"(,"id":)" << id << '}';
    }
}



// wss://api.demo.hitbtc.com/api/3/ws/wallet
// {"method": "spot_subscribe","params": {},"id": 123}

// {"method": "subscribe_wallet_balances","params": {},"id": 7653}

// {"method": "login","params": {"type": "BASIC","api_key": "DjHTMhvOLe8QiKaGJ4EXGYQbzfXLqKcC","secret_key": "0e4x1WxctyiFxbvbcGVXuzawNfO2Ruy6"}}

// {"method": "spot_cancel_order","params": {"client_order_id": "harry225"},"id": 123}

// {"method": "spot_balance_subscribe","params": {"mode": "batches"},"id": 123}

// {"method":"spot_get_orders", "id":123}


// wss://api.demo.hitbtc.com/api/3/ws/trading

// {"method": "spot_new_order","params": {"client_order_id": "harry225","symbol": "BTCUSDT","side": "sell","type": "limit", "time_in_force":"GTC", "quantity": "0.0015","price": "16731"},"id": 123}

// {"method": "spot_replace_order","params": {"client_order_id": "harry222","new_client_order_id": "harry223","quantity": "0.00001","price": "0.02"},"id": 123}
