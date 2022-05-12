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

int getExchangeId(const std::string& aHost, const std::string& aPort, const std::string& aPath, const std::string& aCode) {
    const int failure = -1;
    std::string jsonStr;
    if (!getWebPageBody(aHost, aPort, aPath, jsonStr))
        return failure;
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    if (!doc.IsObject() && !doc.IsArray()) {
        std::cout << "Failed to parse json string " << std::endl;
        return failure;
    }
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject() && item.HasMember("code") && item.HasMember("id")) {
                if (aCode == item["code"].GetString()) {
                    return item["id"].GetInt();
                }
            }
        }
    }
    return failure;
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
            if (item.IsObject() && item.HasMember("externalSymbol") && item.HasMember("internalSymbol")) {
                aSymMap.emplace(item["internalSymbol"].GetString(), item["externalSymbol"].GetString()); 
            }
        }
    }
    return aSymMap.size() > 0;
}

int main() {
    std::cout << "BFX exch id: " << getExchangeId("172.21.5.151", "8001", "/api/assetStores", "BFX") << std::endl;
    std::cout << "LMAXG exch id: " << getExchangeId("172.21.5.151", "8001", "/api/assetStores", "LMAXG") << std::endl;

    ExchSymbolMap symbolMap;
    std::ostringstream oss;
    oss << "/api/v1/aliases/" << 1;
    getExchangeSymbols("172.21.2.170", "9004", oss.str().c_str(), symbolMap);
    std::cout << "Loaded " << symbolMap.size() << " items for Bitfinex. " << std::endl;
}
