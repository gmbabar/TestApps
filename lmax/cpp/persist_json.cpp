#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fstream>

// Function to append data to a JSON file
void appendDataToJsonFile(const std::string& filename, const std::string& clientOrderId,
                           const std::string& nebulaOrderId, const std::string& exchangeOrderId,
                           const std::string& traderId, const std::string& strategy) {
    try {
        // Read the existing JSON file into a property tree
        boost::property_tree::ptree pt;
        std::ifstream input(filename);
        if (input.good()) {
            boost::property_tree::read_json(input, pt);
            input.close();
        }

        // Create a new JSON object with the provided data
        boost::property_tree::ptree newEntry;
        newEntry.put("clientOrderId", clientOrderId);
        newEntry.put("nebulaOrderId", nebulaOrderId);
        newEntry.put("exchangeOrderId", exchangeOrderId);
        newEntry.put("traderId", traderId);
        newEntry.put("strategy", strategy);

        // Append the new object to the property tree
        pt.push_back(std::make_pair("order_data", newEntry));

        // Write the updated property tree back to the JSON file
        std::ofstream output(filename);
        boost::property_tree::write_json(output, pt);
        output.close();

        std::cout << "Data appended to " << filename << " successfully." << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

int main() {
    // Sample data
    std::string clientOrderId = "12345";
    std::string nebulaOrderId = "67890";
    std::string exchangeOrderId = "ABCDEF";
    std::string traderId = "TRADER123";
    std::string strategy = "TrendFollowing";

    // File name to append the JSON data
    std::string filename = "order_data.json";

    // Append data to the JSON file
    appendDataToJsonFile(filename, clientOrderId, nebulaOrderId, exchangeOrderId, traderId, strategy);

    return 0;
}

