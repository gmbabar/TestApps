// JSON simple example
// This example does not handle errors.
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/doc.h"
#include "rapidjson/writer.h"
#include <unordered_map>
#include <chrono>

/*
const char*ptr
bool
long
double
*/


using boost::property_tree::ptree;
using namespace rapidjson;

struct configValue {
    std::string type;
    rapidjson::Value val;   
};




static const char* kTypeNames[] = 
    { "Null", "False", "True", "Object", "Array", "String", "Number" };

void ParseConfig() {
    configValue obj;
    std::unordered_map<std::string,configValue> umap;
    std::string location;
    const char* json = R"(
{"jsonrpc":"2.0","id":6,"result":{
 "trades":[{"trade_seq": 57071736,"trade_id": "84806005","timestamp": 1635044135734,"tick_direction": 0,"state": "filled","self_trade": false,"reduce_only": false,"profit_loss": 0,"price": 61137.5,"post_only": false,"order_type": "limit","order_id": "6773679687","matching_id": null,"mark_price": 61130.79,"liquidity": "T","label": "COID_1001","instrument_name": "BTC-PERPETUAL","index_price": 61085.51,"fee_currency": "BTC","fee": 0.00000818,"direction": "buy","amount": 1000}],
 "order":{"web": false,"time_in_force": "good_til_cancelled","replaced": false,"reduce_only": false,"profit_loss": 0,"price": 61500,"post_only": false,"order_type": "limit","order_state": "filled","order_id": "6773679687","max_show": 1000,"last_update_timestamp": 1635044135734,"label": "COID_1001","is_liquidation": false,"instrument_name": "BTC-PERPETUAL","filled_amount": 1000,"direction": "buy","creation_timestamp": 1635044135734,"commission": 0.00000818,"average_price": 61137.5,"api": true,"amount": 1000}},
"usIn": 1635044135733570,"usOut": 1635044135735180,"usDiff": 1610,"testnet": true}
)";
    Document doc;
    doc.Parse(json);
    // doc["result"].GetType();
    // obj.type = "digit";
    // obj.val = 10;
    // umap["connector.config.depth_limit"] = obj;

    // for (auto itr = umap.begin(); itr != umap.end(); itr++) {
    //     std::cout << itr->first << " = " << itr->second.type << " : " << itr->second.val.GetInt()<< std::endl;
    // }
    
    if (doc.IsObject()) {
        for (auto& m : doc.GetObject()) {
            // obj.type = kTypeNames[m.value.GetType()];
            location += m.name.GetString();
            umap.emplace(location ,obj);
        }

        for(auto &itr : umap) {
            std::cout << itr.first << ":" << std::endl;
        }

    }
    else if (doc.IsArray()) {
        for (auto& m : doc.IsArray()) {
            auto arrVal = doc[m].GetArray();
            for(SizeType i = 0; i < arrVal.Size(); i++) {
                auto val = arrVal[i].GetString();
            }
        }
    }


  
/*    
    if (doc.IsObject()) {
        for (auto& m : doc.GetObject())
            printf("Type of member %s is %s\n",
                m.name.GetString(), kTypeNames[m.value.GetType()]);
    }
    else : parsing failure.
*/
}

// void PrintNodes(boost::property_tree::ptree const& pt, std::string pad ="") {
//     using boost::property_tree::ptree;
//     ptree::const_iterator end = pt.end();
//     for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
//         std::cout << pad << "'" << it->first << "': " << it->second.get_value<std::string>() << std::endl;
//         if (it->second.begin() != it->second.end()) 
//              PrintNodes(it->second, pad+"\t");
//     }   
//     std::cout << pad << "Done." << std::endl;
// }


int main() {
    ParseConfig();
    return 0;
}


 