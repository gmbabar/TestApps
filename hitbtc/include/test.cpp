#include "parser.hpp"

int main() {
    std::string msg = R"({"ch":"trades","update":{"BTCUSDT":[{"t":1661352113934,"i":1872478761,"p":"21460.55","q":"0.00105","s":"buy"},{"t":1661352113934,"i":1872478762,"p":"21460.56","q":"0.07000","s":"buy"}]}})";
    rapidjson::Document doc;
    doc.Parse(msg.c_str());
    SlowParser::parseTrade(doc);
}