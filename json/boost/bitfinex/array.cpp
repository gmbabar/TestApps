#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <iostream>

using namespace std;

void PrintChild(boost::property_tree::ptree const& pt) {
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    int idx = 0;
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        if (it->second.begin() == it->second.end()) {
            switch (idx) {
            case 0:
                cout << "Price: " << it->second.get_value<std::string>() << endl;
                break;
            case 1:
                cout << "Amount: " << it->second.get_value<std::string>() << endl;
                break;
            case 2:
                cout << "Count: " << it->second.get_value<std::string>() << endl;
                break;
            default:
                break;
            }
            ++idx;
        } else {
            PrintChild(it->second);
        }
    }
    if (idx != 0) cout << endl;

} 

void PrintTop(boost::property_tree::ptree const& pt) {
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        if (it->second.begin() == it->second.end()) {
            cout << "Channel: " << it->second.get_value<std::string>() << endl;
            cout << endl;
        } else {
            PrintChild(it->second);
        }
    }
}

int main () {
    std::stringstream ss;
    boost::property_tree::ptree pt;

    // 2. Snapshot
    ss.str("[10092, [[46128,2,0.23387677], [46121,1,0.00191896]]]");
    cout << "Parsing SNAPSHOT: " << ss.str() << endl << endl;
    boost::property_tree::read_json(ss, pt);
    PrintTop(pt);

    // 3. Update
    ss.str("[10092, [46128,2,0.23387677]]");
    cout << "Parsing UPDATE: " << ss.str() << endl << endl;
    boost::property_tree::read_json(ss, pt);
    PrintTop(pt);
}

    
