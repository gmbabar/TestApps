// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>

using namespace rapidjson;

static const char* kTypeNames[] = 
    { "Null", "False", "True", "Object", "Array", "String", "Number" };

void CheckLatency() {
    const char* json = R"(
{"jsonrpc":"2.0","id":6,"result":{
 "trades":[{"trade_seq": 57071736,"trade_id": "84806005","timestamp": 1635044135734,"tick_direction": 0,"state": "filled","self_trade": false,"reduce_only": false,"profit_loss": 0,"price": 61137.5,"post_only": false,"order_type": "limit","order_id": "6773679687","matching_id": null,"mark_price": 61130.79,"liquidity": "T","label": "COID_1001","instrument_name": "BTC-PERPETUAL","index_price": 61085.51,"fee_currency": "BTC","fee": 0.00000818,"direction": "buy","amount": 1000}],
 "order":{"web": false,"time_in_force": "good_til_cancelled","replaced": false,"reduce_only": false,"profit_loss": 0,"price": 61500,"post_only": false,"order_type": "limit","order_state": "filled","order_id": "6773679687","max_show": 1000,"last_update_timestamp": 1635044135734,"label": "COID_1001","is_liquidation": false,"instrument_name": "BTC-PERPETUAL","filled_amount": 1000,"direction": "buy","creation_timestamp": 1635044135734,"commission": 0.00000818,"average_price": 61137.5,"api": true,"amount": 1000}},
"usIn": 1635044135733570,"usOut": 1635044135735180,"usDiff": 1610,"testnet": true}
)";
    Document doc;
    doc.Parse(json);

    for (auto& m : doc.GetObject())
    printf("Type of member %s is %s\n",
        m.name.GetString(), kTypeNames[m.value.GetType()]);
}

int main() {
    CheckLatency();
    return 0;
}
