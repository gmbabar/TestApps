#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <rapidjson/document.h>

inline bool getWebPageBody(const std::string& aHost, const std::string& aPort, const std::string& aPath, std::string &aBody) {
    bool body = false;
    boost::asio::ip::tcp::iostream stm(aHost, aPort);
    if(!stm) {
        std::cout << "Could not connect to " << aHost << "\n";
        return body;
    }
    stm << "GET " << aPath << " HTTP/1.0\r\n"
      << "Host: " << aHost << " \r\n"
      << "Accept: */*\r\n"
      << "Connection: close\r\n\r\n" ;
    std::ostringstream oss;
    for(std::string line; getline(stm, line); ) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.size() == 0)
            body = true;
        else if (body) {
            oss << line;
        }
    }
    aBody = oss.str();
    return body;
}

#include <unordered_map>
typedef std::unordered_map<std::string, std::string> ExchSymbolMap;
bool getExchangeSymbols(const std::string& aHost, const std::string& aPort, const std::string& aPath, ExchSymbolMap &aSymMap) {
    std::string jsonStr;
    if (!getWebPageBody(aHost, aPort, aPath, jsonStr))
        return false;
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    if (!doc.IsObject() && !doc.IsArray()) {
        std::cout << "Failed to parse json string " << std::endl;
        return false;
    }
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject() && item.HasMember("md2Symbol") && item.HasMember("exchangeSymbol")) {
                aSymMap.emplace(item["md2Symbol"].GetString(), item["exchangeSymbol"].GetString()); 
            }
        }
    }
    return aSymMap.size() > 0;
}

int main() {
    // http://172.21.4.120:8070/api/v1/tm/galaxy-assets?galaxyExchangeCode=OKEXF
    ExchSymbolMap symbolMap;
    std::string exchangeCode = "OKEXF";
    std::ostringstream oss;
    oss << "/api/v1/tm/galaxy-assets?galaxyExchangeCode=" << exchangeCode;
    getExchangeSymbols("172.21.4.120", "8070", oss.str().c_str(), symbolMap);
    std::cout << "Loaded " << symbolMap.size() << " items for Okex future. " << std::endl;
}


