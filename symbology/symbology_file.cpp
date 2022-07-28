#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> ExchSymbolMap;
bool getExchangeSymbols(const std::string& aFilePath, ExchSymbolMap &aSymMap) {
    using namespace rapidjson;
    using namespace std;
 
    std::ifstream ifs(aFilePath);
    rapidjson::IStreamWrapper isw(ifs);
 
    rapidjson::Document doc;
    doc.ParseStream(isw);
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

int main(int argc, char *argv[0]) {
    if (argc != 2) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <symbols.json>" << std::endl << std::endl;
    }
    ExchSymbolMap symbolMap;
    getExchangeSymbols(argv[1], symbolMap);
    std::cout << "Loaded " << symbolMap.size() << " items for Okex future. " << std::endl;
}


