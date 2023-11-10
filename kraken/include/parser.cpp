// #include <iostream>
// #include "parser.hpp"

// int main() {
//     std::string msg = R"([4032,{"a":[["0.50000","5983.70160066","1692110352.661259"],["0.51000","6983.70160066","1692110352.661259"]]},{"b":[["0.49600","7238.07711848","1692110352.661436"]],"c":"16020299"},"book-10","FLOW/EUR"])";
//     std::string msg2 = R"([4992,{"a":[["0.000235600","404.82369997","1692110477.213628"],["0.000238500","1127.74718094","1692110477.213635"],["0.000241500","2154.19965260","1692110477.213654"]]},{"b":[["0.000233600","473.33187644","1692110477.213598"],["0.000230600","1031.51242518","1692110477.213609"]],"c":"2806224834"},"book-10","LPT/XBT"])";
//     std::string msg3 = R"([11632,{"b":[["0.88760000","0.00000","1692110476.576231"],["0.00110000","634709.09090","1689712408.545733","r"],["0.88640000","116.01637","1692110476.576245"]],"c":"3222382475"},"book-10","TUSD/EUR"])";
//     std::string msg4 = R"([11632,{"a":[["0.91710000","0.00000","1692110576.779900"],["814.00000000","19.13924","1692015196.214833","r"]],"c":"3890287115"},"book-10","TUSD/EUR"])";
//     std::cout << "RAW MSG: " << msg << std::endl;
//     fastParseBookUpdate(msg);
//     std::cout << "RAW MSG 2: " << msg2 << std::endl;
//     fastParseBookUpdate(msg2);
//     std::cout << "RAW MSG 3: " << msg3 << std::endl;
//     fastParseBookUpdate(msg3);
//     std::cout << "RAW MSG 4: " << msg4 << std::endl;
//     fastParseBookUpdate(msg4);
// }


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

int main() {
    std::string json = R"([[{}], "SOME VALUE", {"a": 1, "b": 2}])";
    std::stringstream ss;
    ss << json;
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    std::cout << "pt: " << pt.size() << std::endl;

    std::cout << pt.get<std::string>("[1]") << std::endl;

    
    // std::cout << "pt: " << pt << std::endl;
}