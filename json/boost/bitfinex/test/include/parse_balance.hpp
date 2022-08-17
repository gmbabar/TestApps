#include <iostream>
#include <boost/property_tree/json_parser/error.hpp>
#include <boost/property_tree/json_parser.hpp>


/*
-------Response

 {"event":"info","version":2,"serverId":"d7d0f7ca-54d7-4625-a8a0-6524c3b5f3eb",
 "platform":{"status":1}}
 {"event":"auth","status":"OK","chanId":0,"userId":2909250,"auth_id":"e652c76d-4b6f-4a3d-9755-25a25a9e0a66",
 "caps":{"orders":{"read":1,"write":1},"account":{"read":1,"write":0},"funding":{"read":1,"write":0},
 "history":{"read":1,"write":0},"wallets":{"read":1,"write":0},"withdraw":{"read":0,"write":0},
 "positions":{"read":1,"write":1},"ui_withdraw":{"read":0,"write":0}}}
 [0,"ps",[["tTESTBTC:TESTUSD","ACTIVE",3.8e-7,38930,0,0,null,null,null,null,null,152677056,null,null,null,0,null,0,null,null]]]
 [0,"ws",[["exchange","AAA",50025,0,null,null,null],
 ["exchange","BBB",50025,0,null,null,null],
 ["exchange","TESTBTC",2.1047932,0,null,null,null],
 ["exchange","TESTUSD",7.757,0,null,null,null],
 ["exchange","TESTUSDT",1001,0,null,null,null],
 ["margin","TESTUSDT",78671.96191916,0,null,null,null],
 ["funding","TESTUSDT",9000,0,null,null,null],
 ["margin","TESTUSDTF0",11000,0,null,null,null],
 ["margin","TESTUSD",99991.87021459,0,null,null,null],
 ["margin","TESTBTC",2,0,null,null,null]]]
 [0,"os",[]][0,"fos",[]][0,"fcs",[]][0,"fls",[]]
 [0,"wu",["exchange","AAA",50025,0,50025,null,null]]
 [0,"wu",["exchange","BBB",50025,0,50025,null,null]]
 [0,"wu",["exchange","TESTBTC",2.1047932,0,2.1047932,null,null]]
 [0,"wu",["exchange","TESTUSD",7.757,0,7.757,null,null]]
 [0,"wu",["exchange","TESTUSDT",1001,0,1001,null,null]]
 [0,"wu",["margin","TESTUSDT",78671.96191916,0,78671.96191916,null,null]]
 [0,"wu",["funding","TESTUSDT",9000,0,9000,null,null]]
 [0,"wu",["margin","TESTUSDTF0",11000,0,11000,null,null]]
 [0,"wu",["margin","TESTUSD",99991.87021459,0,99991.87021459,null,null]]
 [0,"wu",["margin","TESTBTC",2,0,2,null,null]]
 [0,"bu",[397190.90366775,397190.89789745]]
 [0,"pu",["tTESTBTC:TESTUSD","ACTIVE",3.8e-7,38930,0,0,-0.0057883462,-39.00590804007192,0,3.9898064715911756e-8,null,152677056,null,null,null,0,null,0,0,null]]
*/

using boost::property_tree::ptree;

void PrintChild(boost::property_tree::ptree const& pt) {
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    int idx = 0;
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        if (it->second.begin() == it->second.end()) {
            
            std::cout << "values: " << it->second.get_value<std::string>() << std::endl;
       
            ++idx;
        } else {
            PrintChild(it->second);
        }
    }
    if (idx != 0) std::cout << std::endl;

} 

void PrintTop(boost::property_tree::ptree const& pt) {
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        if (it->second.begin() == it->second.end()) {
            std::cout << "Channel: " << it->second.get_value<std::string>() << std::endl;
            std::cout << std::endl;
        } else {
            PrintChild(it->second);
        }
    }
}


inline  std::stringstream GetMsg(std::string str, int &start, int &stop) {
    std::stringstream oss;
    for (int i = start; i <= stop; i++) {
        oss << str[i]; 
    }
    return oss;
    // std::cout << std::endl;
}

void parse_wu(std::stringstream& oss) {
    ptree pt;
    std::stringstream noss;
    int start = oss.str().find(R"( [0,"wu")");
    int stop = oss.str().find(R"(]])",start++);
    noss = GetMsg(oss.str(),start,stop);
    for(int i=0;i<noss.str().size();i++) {
    start = oss.str().find(R"( [0,"wu")",start++);
    stop = oss.str().find(R"(]])",start++);
    stop++;
    noss = GetMsg(oss.str(),start,stop);
    std::cout << "Parsing wallet update: " << noss.str() << std::endl << std::endl;
    // std::cout << "Parsing wallet snapshot: " << oss.str() << std::endl << std::endl;
    if(!(noss.str()==" "))   {
        boost::property_tree::read_json(noss, pt);
        PrintTop(pt);
        }
    }
}
void parse_ws(std::stringstream& oss) {
    ptree pt;
    std::stringstream noss;
    int start;
    int stop;
    start = oss.str().find(R"([0,"ws")");
    stop = oss.str().find(R"(]]])",start);
    stop = stop+2;
    noss = GetMsg(oss.str(),start,stop);
    std::cout << "Parsing wallet snapshot: " << noss.str() << std::endl << std::endl;
    boost::property_tree::read_json(noss, pt);
    PrintTop(pt);
}
void parse_balance(std::stringstream& oss) {
    parse_ws(oss);
    parse_wu(oss);
}
