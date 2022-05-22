// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>
#include <sstream>


using namespace rapidjson;

inline uint64_t getMicrosSinceEpoch() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch()
      ).count();
}

/*** Hello Msg - client side
{
  "id"        : <number>,
  "type"      : "helo",
  "clid"      : <string>,
  "exid"      : <string>,
  "cl_type"   : <string>,
  "omni"      : <string>,
  "init_data" : <string>
} 
***/

/*** Heartbeat Msg - client side
{
  "id"   : <number>,
  "type" : "hrbt"
}
***/

/*** Balances Request Msg - client side
{
  "id"   : <number>,
  "type" : "blrq"
}
***/

/*** Pair Data Msg - client side
{
  "id"   : <number>,
  "type" : "pdrq",
  "quote_curr": <string>
  "base_curr" : <string>
}
***/

/*** Open Orders Request Msg - client side
{
  "id"   : <number>,
  "type" : "oprq"
}
***/

/*** New Order Msg - client side
{
  "id"           : <number>,
  "type"         : "ornw",
  "ord_cl_id"    : <string>,
  "cl_account_id": <string>,
  "ord_type"     : <string>,
  "symbol"       : <string>,
  "amount"       : <number>,
  "price"        : <number>,
  "post_only"    : <number>,
  "hidden"       : <number>,
  "trader_id"    : <string>,
  "strategy"     : <string>,
  "tif"          : <string>,
  "min_amount"   : <number>
}
***/
inline void formatNewOrder(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& anOrdClId, 
        const std::string& anCltAccId, 
        const std::string& aOrdType, 
        const std::string& aSymbol, 
        const double anAmount,
        const double aPrice,
        const unsigned aPostOnly,   // should be bool but protocol states number
        const unsigned aHidden,
        const std::string& aTraderId,
        const std::string& aStrategy,
        const std::string& aTif,
        const double aMinAmount) {
    oss << "{"
    << R"("id":)" << anId
    << R"(,"type":"ornw")"
    << R"(,"ord_cl_id":")" << anOrdClId << R"(")"
    << R"(,"cl_account_id":")"<< anCltAccId << R"(")"
    << R"(,"ord_type":")" << aOrdType << R"(")"
    << R"(,"symbol":")" << aSymbol << R"(")"
    << R"(,"amount":)" << anAmount 
    << R"(,"price":)" << aPrice 
    << R"(,"post_only":)" << aPostOnly 
    << R"(,"hidden":)" << aHidden 
    << R"(,"trader_id":")" << aTraderId << R"(")"
    << R"(,"strategy":")" << aStrategy << R"(")"
    << R"(,"tif":")" << aTif << R"(")"
    << R"(,"min_amount":)" << aMinAmount 
    << "}";
}

inline void parseNewOrder(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetInt64() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ord_cl_id: " << document["ord_cl_id"].GetString() << std::endl;
    std::cout << __func__ << ": cl_account_id: " << document["cl_account_id"].GetString() << std::endl;
    std::cout << __func__ << ": ord_type: " << document["ord_type"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << __func__ << ": amount: " << document["amount"].GetDouble() << std::endl;
    std::cout << __func__ << ": price: " << document["price"].GetDouble() << std::endl;
    std::cout << __func__ << ": post_only: " << document["post_only"].GetInt() << std::endl;
    std::cout << __func__ << ": hidden: " << document["hidden"].GetInt() << std::endl;
    std::cout << __func__ << ": trader_id: " << document["trader_id"].GetString() << std::endl;
    std::cout << __func__ << ": strategy: " << document["strategy"].GetString() << std::endl;
    std::cout << __func__ << ": tif: " << document["tif"].GetString() << std::endl;
    std::cout << __func__ << ": min_amount: " << document["min_amount"].GetDouble() << std::endl; // optional
}

/*** Cancel Order Msg - client side
{
  "id"       : <number>,
  "type"     : "orcn",
  "ord_cl_id": <string>,
  "ord_ex_id": <string>,
  "can_id"   : <number>
}
***/
inline void formatCancelOrder(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& anOrdClId, 
        const std::string& anCltAccId, 
        const uint64_t aCancelId) {
    oss << "{"
    << R"("id":)" << anId
    << R"(,"type":"orcn")"
    << R"(,"ord_cl_id":")" << anOrdClId << R"(")"
    << R"(,"ord_ex_id":")"<< anCltAccId << R"(")"
    << R"(,"can_id":)" << aCancelId
    << "}";
}

inline void parseCancelOrder(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetInt64() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ord_cl_id: " << document["ord_cl_id"].GetString() << std::endl;
    std::cout << __func__ << ": ord_ex_id: " << document["ord_ex_id"].GetString() << std::endl;
    std::cout << __func__ << ": can_id: " << document["can_id"].GetInt64() << std::endl;
}


/*** Cancel Order Batch Msg - client side
{
  "id"       : <number>,
  "type"     : "orcb",
  "orders"   :
    [
        {
          "ord_ex_id"  : <string>,
          "can_id"     : <number>
        },
        {
          "ord_cl_id"  : <string>,
          "can_id"     : <number>
        },
        ...
    ]
}
***/

/*** Acknowledgement Msg - server side
{
  "id"    : <number>,
  "type"  : "ack",
  "ts"    : <number>,
  "refid" : <number>
}
***/

/*** Error Msg - server side
{
  "id"    : <number>,
  "type"  : "err",
  "ts"    : <number>,
  "refid" : <number>,
  "code"  : <number>,
  "msg"   : <string>
}
***/

/*** Exchange Status Msg - server side
{
  "id"    : <number>,
  "type"  : "exst",
  "ts"    : <number>,
  "exid"  : <string>,
  "code"  : <number>,
  "msg"   : <string>
}
***/

/*** Balances Report Msg - server side
{
  "id"         : <number>,
  "type"       : "blrp",
  "ts"         : <number>,
  "account_id" : <number>,
  "bals"       :
    [
      { <curr1> : <number> },
      { <curr2> : <number> },
      ...
      { <currn> : <number> }
    ],
  "bals_ex"    :
  {
      {
        <curr1> :
        {
          <name1> : <value1>,
          <name2> : <value2>,
          ...
          <namen> : <valuen>
        },
        ...
        <currn> :
        {
          <name1> : <value1>,
          <name2> : <value2>,
          ...
          <namen> : <valuen>
        }
      }
    }
}
***/

/*** Pair Data Report Msg - server side
{
  "id"              : <number>,
  "type"            : "pdrp",
  "ts"              : <number>,
  "exid"            : <string>,
  "pairs"           :
    [
      {
        "base_curr"      : <string>,
        "quote_curr"     : <string>,
        "trading_symbol" : <string>,
        "tick_size"      : <number>,
        "tick_unit"      : <number>,
        "step_size"      : <number>,
        "min_size_base"  : <number>,
        "max_size_base"  : <number>,
        "min_size_quote" : <number>,
        "max_size_quote" : <number>,
        "mult"           : <number>,
        "exp_date"       : <string>
      },
      ...
    ]
}
***/

/*** Open Orders Report Msg - server side
{
  "id"              : <number>,
  "type"            : "oprp",
  "ts"              : <number>,
  "exid"            : <string>,
  "account_id"      : <number>,
  "open_orders"     : <number>,
  "orders"          :
    [
      {
        "ord_ex_id"      : <string>,
        "cl_id"          : <string>,
        "symbol"         : <string>,
        "status"         : <string>,
        "orig_amount"    : <string>,
        "curr_amount"    : <string>,
        "price"          : <string>,
        "client_id"      : <string>
      },
      ...
    ]
}
***/

/*** Order Created Msg - server side
{
  "id"        	: <number>,
  "type"      	: "orcr",
  "ts"          : <number>,
  "ord_cl_id" 	: <string>,
  "ord_ex_id" 	: <string>,
  "cl_id"       : <string>,
  "symbol"      : <string>,
  "amount"    	: <number>,
  "price"     	: <number>
}
***/

/*** Order Status Msg - server side
{
  "id"        	: <number>,
  "type"      	: "orst",
  "ts"          : <number>,
  "ord_cl_id" 	: <string>,
  "ord_ex_id" 	: <string>,
  "status"    	: <string>,
  "symbol"      : <string>,
  "amount"    	: <number>,
  "price"     	: <number>,
  "status_text"	: <string>
}
***/

/*** Trade Execution Msg - server side
{
  "id"            : <number>,
  "type"          : "trex",
  "ts"            : <number>,
  "exec_id"       : <number>,
  "ord_cl_id"     : <string>,
  "ord_ex_id"     : <string>,
  "symbol"        : <string>,
  "amount"        : <number>,
  "price"         : <number>,
  "liquidity"     : <string>,
  "fee"           : <string>,
  "fee_currency"  : <string>,
  "bals"  :
    [
      { <curr1> : <number> },
      { <curr2> : <number> },
      ...
      { <currn> : <number> }
    ],
  "bals_ex"    :
  {
      {
        <curr1> :
        {
          <name1> : <value1>,
          <name2> : <value2>,
          ...
          <namen> : <valuen>
        },
        ...
        <currn> :
        {
          <name1> : <value1>,
          <name2> : <value2>,
          ...
          <namen> : <valuen>
        }
      }
    }
}
***/

/*** Order Done Msg - server side
{
  "id"        	: <number>,
  "type"      	: "ordn",
  "ts"          : <number>,
  "ord_cl_id" 	: <string>,
  "ord_ex_id" 	: <string>,
  "symbol"      : <string>,
  "can_id"      : <number>
}
***/

/*** Order Reject Msg - server side
 * NOTE: number is encoded as string from server side as per MA protocol
{
  "id"        	: <number>,
  "type"      	: "orrj",
  "ts"          : <number>,
  "ord_cl_id" 	: <number>,
  "symbol"      : <string>,
  "reason"      : <string>,
  "code"        : <string>
}
***/


/*** Cancel Reject Msg - server side
 * NOTE: number is encoded as string from server side as per MA protocol
{
  "id"        	: <number>,
  "type"      	: "clrj",
  "ts"          : <number>,
  "ord_cl_id" 	: <string>,
  "ord_ex_id" 	: <string>,
  "reason"      : <string>,
  "can_id"      : <number>,
  "code"        : <string>
}
***/
inline void formatCancelReject(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& anOrdClId, 
        const std::string& anOrdExId, 
        const std::string& aReason, 
        const std::string& aCancelId, 
        const std::string& aRejCode) {
    oss << "{"
    << R"("id":")" << anId << R"(")"
    << R"(,"type":"clrj")"
    << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
    << R"(,"ord_cl_id":")" << anOrdClId << R"(")"
    << R"(,"ord_ex_id":")"<< anOrdExId << R"(")"
    << R"(,"reason":")" << aReason << R"(")"
    << R"(,"can_id":")" << aCancelId << R"(")"
    << R"(,"code":")" << aRejCode << R"(")"
    << "}";
}

/**
 * @brief Json cancel reject parsing
 * 
 */
inline void parseCancelReject(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": ord_cl_id: " << document["ord_cl_id"].GetString() << std::endl;
    std::cout << __func__ << ": ord_ex_id: " << document["ord_ex_id"].GetString() << std::endl;
    std::cout << __func__ << ": reason: " << document["reason"].GetString() << std::endl;
    std::cout << __func__ << ": can_id: " << document["can_id"].GetString() << std::endl;
    std::cout << __func__ << ": code: " << document["code"].GetString() << std::endl;
}

// TODO:
// - need to make sure there is no memory leak(s)
// - need to make sure performance is equal or better than stringstream

int main() {
    std::ostringstream oss;
    std::cout << "----------------------- New Order -----------------------" << std::endl;
    formatNewOrder(oss, 12345, "cltOid123", "cltAct123", "L", "BTC/USD", 0.001, 38500.0, 1, 0, "trd123", "trex", "GTC", 0.0001);
    std::cout << "Json: " << oss.str() << std::endl;
    parseNewOrder(oss.str());
    oss.str("");
    std::cout << "----------------------- Cancel Order -----------------------" << std::endl;
    formatCancelOrder(oss, 12345, "cltOid123", "exchOid123", 54321);
    std::cout << "Json: " << oss.str() << std::endl;
    parseCancelOrder(oss.str());
    oss.str("");
    std::cout << "----------------------- Cancel Reject -----------------------" << std::endl;
    formatCancelReject(oss, 123456, "clt0123", "exchOid1234", "Reject reason", "cancelId012", "50100");
    std::cout << "Json: " << oss.str() << std::endl;
    parseCancelReject(oss.str());
    oss.str("");
    return 0;
}
