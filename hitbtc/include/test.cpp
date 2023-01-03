#include "parser.hpp"


int main() {
    // std::string msg = R"({"ch":"orderbook/top/100ms","data":{"BTCUSDT":{"t":1669910025917,"a":"16977.67","A":"0.00352","b":"16972.55","B":"0.03538"}}})";
    // std::string msg1 = R"({"ch":"ticker/1s","data":{"BTCUSDT":{"t":1670238331387,"a":"17290.25","A":"0.10000","b":"17280.80","B":"0.07226","o":"16997.28","c":"17285.54","h":"17413.05","l":"16905.89","v":"19842.66231","q":"340540322.4741842","p":"288.26","P":"1.695918405768452","L":1998467391}}})";
    // std::string msg2 = R"({"ch":"trades","update":{"BTCUSDT":[{"t":1661352113934,"i":1872478761,"p":"21460.55","q":"0.00105","s":"buy"},{"t":1661352113934,"i":1872478762,"p":"21460.56","q":"0.07000","s":"buy"}]}})";
    // std::string msg3 = R"({"ch": "trades","snapshot": {"BTCUSDT": [{"t": 1626861109494,"i": 1555634969,"p": "30881.96","q": "12.66828","s": "buy"}]}})";
    // FastParser::parseTopOfBook(msg);
    // std::cout << "<=======================>" << std::endl;
    // FastParser::parseTicker(msg1);
    // std::cout << "<=======================>" << std::endl;
    // FastParser::parseTradeUPD(msg2);
    // std::cout << "<=======================>" << std::endl;
    // FastParser::parseTradeSnapshot(msg3);


    // std::cout << std::endl << "======================>" << "NOW CALLING FORMATTERS" << "<======================" << std::endl << std::endl;
    // std::vector<std::string> symbols;
    // std::ostringstream oss;
    // std::string id = "123456", bookInt = "100";
    // // "ETHBTC","BTCUSDT"
    // symbols.push_back("ETHBTC");
    // symbols.push_back("BTCUSDT");
    // FormattersPF::formatSubscribeTicker(oss, symbols, id);
    // oss.str("");
    // std::cout << "<=======================>" << std::endl;
    // FormattersPF::formatSubscribeTrade(oss, symbols, id);
    // oss.str("");
    // std::cout << "<=======================>" << std::endl;
    // FormattersPF::formatSubscribeTopOfBook(oss, symbols, id, bookInt);
    // oss.str("");
    // std::cout << "<=======================>" << std::endl;
    // rapidjson::Document Doc;
    // Doc.Parse(msg.c_str());
    // parseTopOfBook(Doc);


    std::ostringstream oss;
    std::string apiKey = R"(DjHTMhvOLe8QiKaGJ4EXGYQbzfXLqKcC)";
    std::string secretkey = R"(0e4x1WxctyiFxbvbcGVXuzawNfO2Ruy6)";
    formattersSpotGateway::formatLogin(oss, apiKey, secretkey);
    std::cout << oss.str() << std::endl;
}