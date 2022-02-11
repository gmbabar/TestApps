#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;


void Print(boost::property_tree::ptree const& pt, std::string pad ="")
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << pad << it->first << ": " << it->second.get_value<std::string>() << std::endl;
        if (it->second.begin() != it->second.end()) 
        	Print(it->second, pad+"\t");
    }
    std::cout << pad << "Done." << endl;
}

int main () {

    string str("{\"event\":\"subscribed\",\"channel\":\"book\",\"chanId\":10092,\"symbol\":\"tETHUSD\",\"prec\":\"P0\",\"freq\":\"F0\",\"len\":\"25\",\"subId\":123,\"pair\":\"ETHUSD\"}");
    std::stringstream ss(str);

    // 1. Subscription
    cout << "Parsing SUBSCRIBE: " << ss.str() << endl;
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);
    Print(pt);
    cout << endl;

    cout << "Accessing known key/value pairs: " << endl;
    cout << "Event: " << pt.get<std::string>("event", "DEF") << endl;

    // 2. Snapshot
    ss.str("[10092, [[46128,2,0.23387677], [46121,1,0.00191896]]]");
    cout << "Parsing SNAPSHOT: " << ss.str() << endl;
    boost::property_tree::read_json(ss, pt);
    Print(pt);
    cout << endl;

    // 3. Update
    ss.str("[10092, [46128,2,0.23387677]]");
    cout << "Parsing UPDATE: " << ss.str() << endl;
    boost::property_tree::read_json(ss, pt);
    Print(pt);
    cout << endl;
}

    
