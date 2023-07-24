#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
void PrintOrderData(boost::property_tree::ptree const& pt, std::string pad ="")
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::string note;
        if (!it->first.empty() && it->second.begin() != it->second.end()) {
            std::cout << pad << "OrderID: " << it->first << ": " << it->second.get_value<std::string>() << std::endl;
            const auto &order = it->second;
            for (const auto &val : order) {
                std::cout << pad << ".\t" << val.first << " : " << val.second.get_value<std::string>() << std::endl;
            }
        }
        else if (it->second.begin() != it->second.end()) {
            std::cout << "Printing order:" << std::endl;
        	PrintOrderData(it->second, pad+".\t");
        }
    }
    std::cout << pad << "Done." << endl;
}

int main() {
    const std::string jsonData = R"(
[[
{"OGTT3Y-C6I3P-XRI6HX": {"avg_price": "34.50000", "status": "open", "vol": "10.00345345",
    "descr": {"ordertype": "limit","pair": "XBT/EUR","price": "34.50000","type": "sell"}}},
{"OGTT3Y-C6I3P-XRI6HY": {"status": "closed"}},
{"OGTT3Y-C6I3P-XRI6HZ": {"status": "closed"}}
],
"openOrders",
{"sequence": 59342}
]
        )";

    try {
        boost::property_tree::ptree pt;
        std::istringstream jsonStream(jsonData);

        // Read the JSON data into the property tree
        boost::property_tree::read_json(jsonStream, pt);

        // Check if the root node is an array or key-value pair
        if (!pt.empty() && pt.begin()->first.empty()) {
            std::cout << "The JSON data is an array." << std::endl;
            std::cout << __func__ << ": SIZE: " << pt.size() << std::endl;
            const auto &typeItr = ++pt.begin();
            std::cout << __func__ << ": TYPE: " << typeItr->second.get_value<std::string>() << std::endl;
            const auto &orders = pt.begin()->second;
            PrintOrderData(orders);
            
        } else {
            std::cout << "The JSON data is UNKNOWN key-value pair." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

