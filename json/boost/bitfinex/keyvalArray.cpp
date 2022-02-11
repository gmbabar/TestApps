
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

using boost::property_tree::ptree;

int main() {
    std::string ss = "{ \"id\" : \"123\", \"number\" : \"456\", \"stuff\" : [{ \"name\" : \"test\" }, { \"name\" : \"some\" }, { \"name\" : \"stuffs\" }] }";

    ptree pt;
    std::istringstream is(ss);
    read_json(is, pt);

    std::cout << "id:     " << pt.get<std::string>("id") << "\n";
    std::cout << "number: " << pt.get<std::string>("number") << "\n";
    for (auto& e : pt.get_child("stuff")) {
        std::cout << "stuff name: " << e.second.get<std::string>("name") << "\n";
    }
}


