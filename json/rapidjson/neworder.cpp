// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>

using namespace rapidjson;

void CheckLatency() {
    const char* json = R"(
{"jsonrpc":"2.0","id":6,"result":{
 "trades":[{"trade_seq": 57071736,"trade_id": "84806005","timestamp": 1635044135734,"tick_direction": 0,"state": "filled","self_trade": false,"reduce_only": false,"profit_loss": 0,"price": 61137.5,"post_only": false,"order_type": "limit","order_id": "6773679687","matching_id": null,"mark_price": 61130.79,"liquidity": "T","label": "COID_1001","instrument_name": "BTC-PERPETUAL","index_price": 61085.51,"fee_currency": "BTC","fee": 0.00000818,"direction": "buy","amount": 1000}],
 "order":{"web": false,"time_in_force": "good_til_cancelled","replaced": false,"reduce_only": false,"profit_loss": 0,"price": 61500,"post_only": false,"order_type": "limit","order_state": "filled","order_id": "6773679687","max_show": 1000,"last_update_timestamp": 1635044135734,"label": "COID_1001","is_liquidation": false,"instrument_name": "BTC-PERPETUAL","filled_amount": 1000,"direction": "buy","creation_timestamp": 1635044135734,"commission": 0.00000818,"average_price": 61137.5,"api": true,"amount": 1000}},
"usIn": 1635044135733570,"usOut": 1635044135735180,"usDiff": 1610,"testnet": true}
)";
    Document d;
    d.Parse(json);

    // 2. Get level-1 values.
    // const auto& rpc = d["jsonrpc"].GetString();
    d["jsonrpc"].GetString();

    // const auto& id = d["id"].GetInt();
    d["id"].GetInt();

    const Value& res = d["result"];
    // std::cout << "res:= object? " << std::boolalpha << res.IsObject() << std::endl;
    // std::cout << "res:= array? " << std::boolalpha << res.IsArray() << std::endl;
    // std::cout << std::endl;
    res.IsObject();
    res.IsArray();


    // NOTE: only way to parse through all members is either write a Handler class
    //       and implement all types  -or- handler + Reader::IterativeParseNext 
    const Value& trds = res["trades"];
    // std::cout << "trd:= object? " << std::boolalpha << trds.IsObject() << std::endl;
    // std::cout << "trd:= array? " << std::boolalpha << trds.IsArray() << std::endl;
    for (SizeType i = 0; i < trds.Size(); i++)  { // rapidjson uses SizeType instead of size_t.
        // printf("trds[%d] trade_seq := %d\n", i, trds[i]["trade_seq"].GetInt());
        // printf("trds[%d] state := %s\n", i, trds[i]["state"].GetString());
        // printf("trds[%d] price := %f\n", i, trds[i]["price"].GetDouble());
        // printf("trds[%d] order_id := %s\n", i, trds[i]["order_id"].GetString());
        // printf("trds[%d] label := %s\n", i, trds[i]["label"].GetString());
        // printf("trds[%d] liquidity := %s\n", i, trds[i]["liquidity"].GetString());
        // printf("trds[%d] instrument_name := %s\n", i, trds[i]["instrument_name"].GetString());
        // printf("trds[%d] amount := %f\n", i, trds[i]["amount"].GetDouble());

        trds[i]["trade_seq"].GetInt();
        trds[i]["state"].GetString();
        trds[i]["price"].GetDouble();
        trds[i]["order_id"].GetString();
        trds[i]["label"].GetString();
        trds[i]["liquidity"].GetString();
        trds[i]["instrument_name"].GetString();
        trds[i]["amount"].GetDouble();
    }
    // std::cout << std::endl;

    const Value& ords = res["order"];
    // std::cout << "time_in_force:= " << ords["time_in_force"].GetString() << std::endl;
    // std::cout << "price:= " << ords["price"].GetDouble() << std::endl;
    // std::cout << "order_type:= " << ords["order_type"].GetString() << std::endl;
    // std::cout << "order_state:= " << ords["order_state"].GetString() << std::endl;
    // std::cout << "order_id:= " << ords["order_id"].GetString() << std::endl;
    // std::cout << "instrument_name:= " << ords["instrument_name"].GetString() << std::endl;
    // std::cout << "direction:= " << ords["direction"].GetString() << std::endl;
    // std::cout << "amount:= " << ords["amount"].GetDouble() << std::endl;

    ords["time_in_force"].GetString();
    ords["price"].GetDouble();
    ords["order_type"].GetString();
    ords["order_state"].GetString();
    ords["order_id"].GetString();
    ords["instrument_name"].GetString();
    ords["direction"].GetString();
    ords["amount"].GetDouble();
}

int main() {
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for(int i = 0; i<1E4; i++) CheckLatency();
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::milliseconds timeSpan = std::chrono::duration_cast<std::chrono::milliseconds>(t2.time_since_epoch() - t1.time_since_epoch());
    std::cout << "The time To Parse Was : " << timeSpan.count() << " ms" << std::endl;
    return 0;
}