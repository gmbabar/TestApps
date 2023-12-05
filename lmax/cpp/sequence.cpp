#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// Function to save sequence numbers to a JSON file
void saveSequenceNumbers(const std::string& filename, int inbound, int outbound) {
    // Create a property tree
    boost::property_tree::ptree pt;

    // Add the sequence numbers to the property tree
    pt.put("sequence_numbers.inbound", inbound);
    pt.put("sequence_numbers.outbound", outbound);

    // Write the property tree to a JSON file
    boost::property_tree::write_json(filename, pt);

    std::cout << "Sequence numbers saved to " << filename << std::endl;
}

int store() {
    // Sample sequence numbers
    int inboundSequence = 100;
    int outboundSequence = 200;

    // File name to save the JSON data
    std::string filename = "sequence_numbers.json";

    // Save sequence numbers to the JSON file
    saveSequenceNumbers(filename, inboundSequence, outboundSequence);

    return 0;
}

// Function to load sequence numbers from a JSON file
bool loadSequenceNumbers(const std::string& filename, int& inbound, int& outbound) {
    // Create a property tree
    boost::property_tree::ptree pt;

    try {
        // Read the JSON file into the property tree
        boost::property_tree::read_json(filename, pt);

        // Extract sequence numbers from the property tree
        inbound = pt.get<int>("sequence_numbers.inbound");
        outbound = pt.get<int>("sequence_numbers.outbound");

        std::cout << "Sequence numbers loaded successfully." << std::endl;
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Error reading JSON file: " << ex.what() << std::endl;
        return false;
    }
}

int load() {
    // Variables to store loaded sequence numbers
    int loadedInboundSequence, loadedOutboundSequence;

    // File name to load the JSON data
    std::string filename = "sequence_numbers.json";

    // Load sequence numbers from the JSON file
    if (loadSequenceNumbers(filename, loadedInboundSequence, loadedOutboundSequence)) {
        // Display the loaded sequence numbers
        std::cout << "Loaded Inbound Sequence: " << loadedInboundSequence << std::endl;
        std::cout << "Loaded Outbound Sequence: " << loadedOutboundSequence << std::endl;
    } else {
        std::cerr << "Failed to load sequence numbers." << std::endl;
    }

    return 0;
}

int main () {

    // Load Sequnce Numbers.
    load();
    // Store Sequence Numbers.
    store();
    // Load Sequnce Numbers.
    load();
}
