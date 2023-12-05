#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

struct OrderData {
    std::string clientOrderId;
    std::string nebulaOrderId;
    std::string exchangeOrderId;
    std::string traderId;
    std::string strategy;
};

// Function to dump order data to a stringstream
std::stringstream dumpOrderData(const OrderData& orderData) {
    std::stringstream ss;
    ss << "{"
       << "\"clientOrderId\":\"" << orderData.clientOrderId << "\","
       << "\"nebulaOrderId\":\"" << orderData.nebulaOrderId << "\","
       << "\"exchangeOrderId\":\"" << orderData.exchangeOrderId << "\","
       << "\"traderId\":\"" << orderData.traderId << "\","
       << "\"strategy\":\"" << orderData.strategy << "\""
       << "}";
    return ss;
}

// Function to append stringstream to a file
void appendStringStreamToFile(const std::string& filename, std::stringstream& ss) {
    std::ofstream outputFile(filename, std::ios::app);
    if (outputFile.is_open()) {
        // Append the content of the stringstream to the file
        outputFile << ss.str() << std::endl;
        outputFile.close();
        std::cout << "Order data appended to " << filename << std::endl;
    } else {
        std::cerr << "Error opening file for appending: " << filename << std::endl;
    }
}

int main() {
    // Sample order data
    OrderData order = {
        "12345",
        "67890",
        "EX123",
        "TRADER001",
        "SCALPING"
    };

    // File name to append the JSON data
    std::string filename = "order_data.txt";

    // Dump order data to a stringstream
    std::stringstream orderStringStream = dumpOrderData(order);

    // Append stringstream to the file
    appendStringStreamToFile(filename, orderStringStream);

    return 0;
}


