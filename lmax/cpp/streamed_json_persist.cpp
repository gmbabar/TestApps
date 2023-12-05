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

// Function to dump order data into a stringstream
std::stringstream dumpOrderData(const OrderData& orderData) {
    // Create a stringstream
    std::stringstream ss;

    // Create a property tree for the order data
    boost::property_tree::ptree pt;
    pt.put("clientOrderId", orderData.clientOrderId);
    pt.put("nebulaOrderId", orderData.nebulaOrderId);
    pt.put("exchangeOrderId", orderData.exchangeOrderId);
    pt.put("traderId", orderData.traderId);
    pt.put("strategy", orderData.strategy);

    // Write the property tree to the stringstream
    boost::property_tree::write_json(ss, pt);

    return ss;
}

// Function to append stringstream contents to a file
void appendStringStreamToFile(const std::string& filename, std::stringstream& ss) {
    // Open the file in append mode
    std::ofstream outfile(filename, std::ios_base::app);

    // Append stringstream contents to the file
    outfile << ss.str();

    std::cout << "Order data appended to " << filename << std::endl;
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
    std::string filename = "order_data.json";

    // Dump order data into a stringstream
    std::stringstream ss = dumpOrderData(order);

    // Append stringstream contents to the JSON file
    appendStringStreamToFile(filename, ss);

    return 0;
}

