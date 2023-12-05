#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <map>

struct OrderData {
    std::string clientOrderId;
    std::string nebulaOrderId;
    std::string exchangeOrderId;
    std::string traderId;
    std::string strategy;
};

// Function to load JSON data from a stringstream and insert into std::map
void loadJsonData(const std::string& jsonStr, std::map<std::string, OrderData>& orderMap) {
    std::stringstream ss(jsonStr);
    std::cout << __func__ << ": " << jsonStr << std::endl;

    boost::property_tree::ptree pt;
    try {
        boost::property_tree::read_json(ss, pt);

        OrderData order;
        order.clientOrderId = pt.get<std::string>("clientOrderId");
        order.nebulaOrderId = pt.get<std::string>("nebulaOrderId");
        order.exchangeOrderId = pt.get<std::string>("exchangeOrderId");
        order.traderId = pt.get<std::string>("traderId");
        order.strategy = pt.get<std::string>("strategy");

        // Insert into the map with clientOrderId as the key
        //orderMap.emplace(order.clientOrderId, order);
        orderMap[order.clientOrderId] = order;
    } catch (const std::exception& ex) {
        std::cerr << "Error reading JSON data: " << ex.what() << std::endl;
    }
}

int main() {
    // File name to read the JSON data
    std::string filename = "order_data.txt";

    // Map to store OrderData by clientOrderId
    std::map<std::string, OrderData> orderMap;

    // Read the file line by line
    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            // Load JSON data from each line into the map
            loadJsonData(line, orderMap);
        }
        inputFile.close();

        // Display the loaded data
        for (const auto& pair : orderMap) {
            std::cout << "ClientOrderId: " << pair.first;
            std::cout << ", NebulaOrderId: " << pair.second.nebulaOrderId;
            std::cout << ", ExchangeOrderId: " << pair.second.exchangeOrderId;
            std::cout << ", TraderId: " << pair.second.traderId;
            std::cout << ", Strategy: " << pair.second.strategy << "\n";
        }
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    return 0;
}

