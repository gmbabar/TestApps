// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace rapidjson;

inline uint64_t getMicrosSinceEpoch() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch()
      ).count();
}
// ********************************************************
// TODO: Need to add validation on each value while parsing:
//  - does value exist
//  - does type match
//  - respond if field is required.
// ********************************************************

/*** Hello Msg - client side
{
  "id"        : <number>,
  "type"      : "helo",
  "clid"      : <string>,
  "exid"      : <string>,
  "cl_type"   : <string>,   // optional
  "omni"      : <string>,   // optional, type should have been bool
  "init_data" : <string>
} 
***/
inline void formatHelloMsg(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& aClId, 
        const std::string& anExId,
        const std::string& aClType,
        const std::string& isOmni,
        const std::string& anInitData) {
    oss << "{"
        << R"("id":)" << anId
        << R"(,"type":"helo")"
        << R"(,"clid":")" << aClId << R"(")"
        << R"(,"exid":")" << anExId << R"(")";
    if (!aClType.empty())
      oss << R"(,"cl_type":")" << aClType << R"(")";
    oss << R"(,"omni":")" << isOmni << R"(")"
        << R"(,"init_data":")" << anInitData << R"(")"
        << "}";
}

inline void parseHelloMsg(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetInt64() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": clid: " << document["clid"].GetString() << std::endl;
    std::cout << __func__ << ": exid: " << document["exid"].GetString() << std::endl;
    if (document.HasMember("cl_type"))
      std::cout << __func__ << ": cl_type: " << document["cl_type"].GetString() << std::endl;
    std::cout << __func__ << ": omni: " << (document.HasMember("omni") ? document["omni"].GetString() : "false") << std::endl;
    std::cout << __func__ << ": init_data: " << document["init_data"].GetString() << std::endl;
}


/*** Heartbeat Msg - client side
{
  "id"   : <number>,
  "type" : "hrbt"
}
***/

inline void formatHeartbeat(std::ostringstream &oss,
        const uint64_t anId) {
    oss << "{"
        << R"("id":)" << anId
        << R"(,"type":"hrbt")"
        << "}";
}

inline void parseHeartbeat(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetInt64() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
}


/*** Balances Request Msg - client side
{
  "id"   : <number>,
  "type" : "blrq"
}
***/
inline void formatBalanceReq(std::ostringstream &oss,
        const uint64_t anId) {
    oss << "{"
        << R"("id":)" << anId
        << R"(,"type":"blrq")"
        << "}";
}

inline void parseBalanceReq(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetInt64() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
}

/*** Pair Data Msg - client side
{
  "id"   : <number>,
  "type" : "pdrq",
  "quote_curr": <string>
  "base_curr" : <string>
}
***/
inline void formatPairDataReq(std::ostringstream &oss,
        const uint64_t anId,
        const std::string &aQuoteCcy,
        const std::string &aBaseCcy) {
    oss << "{"
        << R"("id":)" << anId
        << R"(,"type":"pdrq")"
        << R"(,"quote_curr":")" << aQuoteCcy << R"(")"
        << R"(,"base_curr":")" << aBaseCcy << R"(")"

        << "}";
}

inline void parsePairDataReq(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetInt64() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": quote_curr: " << document["quote_curr"].GetString() << std::endl;
    std::cout << __func__ << ": base_curr: " << document["base_curr"].GetString() << std::endl;
}


/*** Open Orders Request Msg - client side
{
  "id"   : <number>,
  "type" : "oprq"
}
***/
inline void formatOpenOrdersReq(std::ostringstream &oss,
        const uint64_t anId) {
    oss << "{"
        << R"("id":)" << anId
        << R"(,"type":"oprq")"
        << "}";
}

inline void parseOpenOrdersReq(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetInt64() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
}


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
        << R"(,"type":"orcn")";

    if (!anOrdClId.empty())
      oss << R"(,"ord_cl_id":")" << anOrdClId << R"(")";

    if (!anCltAccId.empty())
      oss << R"(,"ord_ex_id":")"<< anCltAccId << R"(")";

    oss << R"(,"can_id":)" << aCancelId
        << "}";
}

inline void parseCancelOrder(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetInt64() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ord_cl_id: " << (document.HasMember("ord_cl_id") ? document["ord_cl_id"].GetString() : "") << std::endl;
    std::cout << __func__ << ": ord_ex_id: " << (document.HasMember("ord_ex_id") ? document["ord_ex_id"].GetString() : "") << std::endl;
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
struct CancelOrderData {
  CancelOrderData(const std::string &aClientOid,
      const std::string &aExchOid,
      const uint64_t aCanId)
      : m_clientOrderId(aClientOid),
      m_exchOrderId(aExchOid),
      m_cancelId(aCanId) {
      }

  void formatCancelOrderData(std::ostringstream &oss) const {
    oss << "{"
        << R"("can_id":)" << m_cancelId;
    if (!m_clientOrderId.empty())
      oss << R"(,"ord_cl_id":")" << m_clientOrderId << R"(")";
    if (!m_exchOrderId.empty())
      oss << R"(,"ord_ex_id":")" << m_exchOrderId << R"(")";
    oss << "}";
  }

private:
  const std::string m_clientOrderId;
  const std::string m_exchOrderId;
  const uint64_t m_cancelId;
};

inline void formatCancelOrderBatch(std::ostringstream &oss,
        const uint64_t anId, 
        const std::vector<CancelOrderData> cancelOrders) {
    bool once = true;
    oss << "{"
        << R"("id":)" << anId
        << R"(,"type":"orcb")"
        << R"(,"orders":[)";
    for (const auto &order : cancelOrders) {
      if (once)
        once = false;
      else
        oss << ",";
      order.formatCancelOrderData(oss);
    }
    oss << "]}";
}

inline void parseCancelOrderBatch(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetInt64() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": orders: " << std::endl;
    if (document.HasMember("orders") && document["orders"].IsArray()) {
      for (auto& item : document["orders"].GetArray()) {
        if (item.IsObject()) {
          for (auto& m : item.GetObject()) {
            auto itemName = m.name.GetString();
            if (std::string("can_id") == itemName)
              std::cout << __func__ << "\t" << m.name.GetString() << ": " 
                        << m.value.GetInt64() << std::endl;
            else
              std::cout << __func__ << "\t" << m.name.GetString() << ": " 
                        << m.value.GetString() << std::endl;
          }
        }
      }
  }
}


/*** Acknowledgement Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
{
  "id"    : <number>,
  "type"  : "ack",
  "ts"    : <number>,
  "refid" : <number>
}
***/
inline void formatAckMsg(std::ostringstream &oss,
      const uint64_t anId, 
      const uint64_t aRefId) {
    bool once = true;
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"exst")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"refid":")" << aRefId << R"(")"
        << "}";
}

inline void parseAckMsg(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": refid: " << document["refid"].GetString() << std::endl;
}


/*** Error Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
{
  "id"    : <number>,
  "type"  : "err",
  "ts"    : <number>,
  "refid" : <number>,
  "code"  : <number>,
  "msg"   : <string>
}
***/
inline void formatErrorMsg(std::ostringstream &oss,
      const uint64_t anId, 
      const uint64_t aRefId, 
      const uint32_t aCode, 
      const std::string& aMsg) {
    bool once = true;
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"err")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"refid":")" << aRefId << R"(")"
        << R"(,"code":")" << aCode << R"(")"
        << R"(,"msg":")" << aMsg << R"(")"
        << "}";
}

inline void parseErrorMsg(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": refid: " << document["refid"].GetString() << std::endl;
    std::cout << __func__ << ": code: " << document["code"].GetString() << std::endl;
    std::cout << __func__ << ": msg: " << document["msg"].GetString() << std::endl;
}


/*** Exchange Status Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
{
  "id"    : <number>,
  "type"  : "exst",
  "ts"    : <number>,
  "exid"  : <string>,
  "code"  : <number>,
  "msg"   : <string>
}
***/
inline void formatExchangeStatus(std::ostringstream &oss,
      const uint64_t anId, 
      const std::string& anExchId, 
      const uint32_t aCode, 
      const std::string& aMsg) {
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"exst")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"exid":")" << anExchId << R"(")"
        << R"(,"code":")" << aCode << R"(")"
        << R"(,"msg":")" << aMsg << R"(")"
        << "}";
}

inline void parseExchangeStatus(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": exid: " << document["exid"].GetString() << std::endl;
    std::cout << __func__ << ": code: " << document["code"].GetString() << std::endl;
    std::cout << __func__ << ": msg: " << document["msg"].GetString() << std::endl;
}


/*** Balances Report Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
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
inline void formatBalancesReport(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& anAcctId, 
        const std::unordered_map<std::string, std::string> &aCcyMap) {
    bool once = true;
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"trex")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"account_id":")" << anAcctId << R"(")"
        << R"(,"bals":[)";
    for (const auto& kvp : aCcyMap) {
        oss << (once ? "" : ",") 
            << R"({")" << kvp.first <<  R"(":")" << kvp.second << R"("})";
        once = false;
    }
    oss << R"(])"
        << R"(,"bals_ex":[])"   // optional value for future
        << "}";
}

inline void parseBalancesReport(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": account_id: " << document["account_id"].GetString() << std::endl;
    std::cout << __func__ << ": bals: " << std::endl;
    if (document.HasMember("bals") && document["bals"].IsArray()) {
        for (auto& item : document["bals"].GetArray()) {
            if (item.IsObject()) {
              for (auto& m : item.GetObject())
                std::cout << __func__ << "\t" << m.name.GetString() << ": " << m.value.GetString() << std::endl;
            }
        }
    }
}


/*** Pair Data Report Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
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
struct PairData {
  PairData(const std::string &aBaseCcy,
    const std::string &aQuoteCcy,
    const std::string &aTradingSymbol,
    const double aTickSize,
    const unsigned aTickUnit,
    const double aStepSize,
    const double aMinBaseSize,
    const double aMaxBaseSize,
    const double aMinQuoteSize,
    const double aMaxQuoteSize,
    const double aMultiplier,
    const std::string &aExpDate) :
      m_baseCcy(aBaseCcy),
      m_quoteCcy(aQuoteCcy),
      m_tradingSymbol(aTradingSymbol),
      m_tickSize(aTickSize),
      m_tickUnit(aTickUnit),
      m_stepSize(aStepSize),
      m_minBaseSize(aMinBaseSize),
      m_maxBaseSize(aMaxBaseSize),
      m_minQuoteSize(aMinQuoteSize),
      m_maxQuoteSize(aMaxQuoteSize),
      m_multiplier(aMultiplier),
      m_expDate(aExpDate) {
  }

  void formatPairData(std::ostringstream &oss) const {
    oss << R"({)"
        << R"("base_curr":")" << m_baseCcy << R"(")"
        << R"(,"quote_curr":")" << m_quoteCcy << R"(")"
        << R"(,"trading_symbol":")" << m_tradingSymbol << R"(")"
        << R"(,"tick_size":")" << m_tickSize << R"(")"
        << R"(,"tick_unit":")" << m_tickUnit << R"(")"
        << R"(,"step_size":")" << m_stepSize << R"(")"
        << R"(,"min_size_base":")" << m_minBaseSize << R"(")"
        << R"(,"max_size_base":")" << m_maxBaseSize << R"(")"
        << R"(,"min_size_quote":")" << m_minQuoteSize << R"(")"
        << R"(,"max_size_quote":")" << m_maxQuoteSize << R"(")"
        << R"(,"mult":")" << m_multiplier << R"(")"
        << R"(,"exp_date":")" << m_expDate << R"(")"
        << R"(})";
  }

private:
  const std::string m_baseCcy;
  const std::string m_quoteCcy;
  const std::string m_tradingSymbol;
  const double m_tickSize;
  const unsigned m_tickUnit;
  const double m_stepSize;
  const double m_minBaseSize;
  const double m_maxBaseSize;
  const double m_minQuoteSize;
  const double m_maxQuoteSize;
  const double m_multiplier;
  const std::string m_expDate;
};

inline void formatPairDataReport(std::ostringstream &oss,
    const uint64_t anId, 
    const std::string& anExId, 
    const std::vector<PairData> &aDataPairs) {
    bool once = true;
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"pdrp")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"exid":")" << anExId << R"(")"
        << R"(,"pairs":[)";
    for (const auto &dataPair : aDataPairs) {
      if (once)
        once = false;
      else
        oss << ",";
      dataPair.formatPairData(oss);
    }
    oss << R"(])"   // pairs
        << "}";
}

inline void parsePairDataReport(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": exid: " << document["exid"].GetString() << std::endl;
    std::cout << __func__ << ": pairs: " << std::endl;
    if (document.HasMember("pairs") && document["pairs"].IsArray()) {
        for (auto& item : document["pairs"].GetArray()) {
            if (item.IsObject()) {
                for (auto& m : item.GetObject())
                    std::cout << __func__ << "\t" << m.name.GetString() << ": " << m.value.GetString() << std::endl;
            }
        }
    }
}

/*** Open Orders Report Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
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
        "client_id"      : <string>   // both 'cl_id'/'client_id' are pointing to same
      },
      ...
    ]
}
***/
struct OpenOrder {
  OpenOrder(const std::string &exchOid,
    const std::string &clientId,
    const std::string &symbol,
    const std::string &status,
    const double originalAmount,
    const double currentAmount,
    const double price) :
      m_exchOid(exchOid),
      m_clientId(clientId),
      m_symbol(symbol),
      m_status(status),
      m_originalAmount(originalAmount),
      m_currentAmount(currentAmount),
      m_price(price) {
  }

  void formatOpenOrder(std::ostringstream &oss) const {
    oss << R"({)"
        << R"("ord_ex_id":")" << m_exchOid << R"(")"
        << R"(,"cl_id":")" << m_clientId << R"(")"
        << R"(,"symbol":")" << m_symbol << R"(")"
        << R"(,"status":")" << m_status << R"(")"
        << R"(,"orig_amount":")" << m_originalAmount << R"(")"
        << R"(,"curr_amount":")" << m_currentAmount << R"(")"
        << R"(,"price":")" << m_price << R"(")"
        << R"(})";
  }

private:
  const std::string m_exchOid;
  const std::string m_clientId;
  const std::string m_symbol;
  const std::string m_status;
  const double m_originalAmount;
  const double m_currentAmount;
  const double m_price;
};

inline void formatOpenOrdersReport(std::ostringstream &oss,
      const uint64_t anId, 
      const std::string& anExId, 
      const uint64_t anAccountId, 
      const uint32_t anOpenOrdersCount,
      const std::vector<OpenOrder> &orders) {
  bool once = true;
  oss << "{"
      << R"("id":")" << anId << R"(")"
      << R"(,"type":"oprp")"
      << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
      << R"(,"exid":")" << anExId << R"(")"
      << R"(,"account_id":")" << anAccountId << R"(")"
      << R"(,"open_orders":")"<< anOpenOrdersCount << R"(")"
      << R"(,"orders":[)";
  for (const auto &order : orders) {
    if (once)
      once = false;
    else 
      oss << ",";
    order.formatOpenOrder(oss);
  }
  oss << R"(])"
      << "}";
}

inline void parseOpenOrdersReport(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": exid: " << document["exid"].GetString() << std::endl;
    std::cout << __func__ << ": account_id: " << document["account_id"].GetString() << std::endl;
    std::cout << __func__ << ": open_orders: " << document["open_orders"].GetString() << std::endl;
    std::cout << __func__ << ": orders: " << std::endl;
    if (document.HasMember("orders") && document["orders"].IsArray()) {
        for (auto& item : document["orders"].GetArray()) {
            if (item.IsObject()) {
              for (auto& m : item.GetObject())
                std::cout << __func__ << "\t" << m.name.GetString() << ": " << m.value.GetString() << std::endl;
            }
        }
    }
}


/*** Order Created Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
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
inline void formatOrderCreated(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& anOrdClId, 
        const std::string& anOrdExchId, 
        const std::string& aClId, 
        const std::string& aSymbol, 
        const double anAmount, 
        const double aPrice) {
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"orcr")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"ord_cl_id":")" << anOrdClId << R"(")"
        << R"(,"ord_ex_id":")" << anOrdExchId << R"(")"
        << R"(,"cl_id":")"<< aClId << R"(")"
        << R"(,"symbol":")"<< aSymbol << R"(")"
        << R"(,"amount":")"<< anAmount << R"(")"
        << R"(,"price":")"<< aPrice << R"(")"
        << "}";
}

inline void parseOrderCreated(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": ord_cl_id: " << document["ord_cl_id"].GetString() << std::endl;
    std::cout << __func__ << ": ord_ex_id: " << document["ord_ex_id"].GetString() << std::endl;
    std::cout << __func__ << ": cl_id: " << document["cl_id"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << __func__ << ": amount: " << document["amount"].GetString() << std::endl;
    std::cout << __func__ << ": price: " << document["price"].GetString() << std::endl;
}

/*** Order Status Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
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
inline void formatOrderStatus(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& anOrdClId, 
        const std::string& anOrdExchId, 
        const std::string& aStatus, 
        const std::string& aSymbol, 
        const double anAmount, 
        const double aPrice, 
        const std::string& aText) {
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"orst")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"ord_cl_id":")" << anOrdClId << R"(")"
        << R"(,"ord_ex_id":")" << anOrdExchId << R"(")"
        << R"(,"status":")"<< aStatus << R"(")"
        << R"(,"symbol":")"<< aSymbol << R"(")"
        << R"(,"amount":")"<< anAmount << R"(")"
        << R"(,"price":")"<< aPrice << R"(")"
        << R"(,"status_text":")" << aText << R"(")"
        << "}";
}

inline void parseOrderStatus(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": ord_cl_id: " << document["ord_cl_id"].GetString() << std::endl;
    std::cout << __func__ << ": ord_ex_id: " << document["ord_ex_id"].GetString() << std::endl;
    std::cout << __func__ << ": status: " << document["status"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << __func__ << ": amount: " << document["amount"].GetString() << std::endl;
    std::cout << __func__ << ": price: " << document["price"].GetString() << std::endl;
    std::cout << __func__ << ": status_text: " << document["status_text"].GetString() << std::endl;
}

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
  "fee"           : <string>,   // shouldn't this be double?
  "fee_currency"  : <string>,
  "bals"  :
    [                           // should have been simple key/val instead of key/bal objects with one member each
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
inline void formatTradeExecution(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& anExecId, 
        const std::string& anOrdClId, 
        const std::string& anOrdExchId, 
        const std::string& aSymbol, 
        const double anAmount, 
        const double aPrice, 
        const std::string& aLiquidity,
        const double aFee,
        const std::string& aFeeCcy,
        const std::unordered_map<std::string, std::string> &aCcyMap
        ) {
    bool once = true;
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"trex")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"exec_id":")" << anExecId << R"(")"
        << R"(,"ord_cl_id":")" << anOrdClId << R"(")"
        << R"(,"ord_ex_id":")" << anOrdExchId << R"(")"
        << R"(,"symbol":")"<< aSymbol << R"(")"
        << R"(,"amount":")"<< anAmount << R"(")"
        << R"(,"price":")"<< aPrice << R"(")"
        << R"(,"liquidity":")" << aLiquidity << R"(")"
        << R"(,"fee":")" << aFee << R"(")"
        << R"(,"fee_currency":")" << aFeeCcy << R"(")"
        << R"(,"bals":[)";
    for (const auto& kvp : aCcyMap) {
        oss << (once ? "" : ",") 
            << R"({")" << kvp.first <<  R"(":")" << kvp.second << R"("})";
        once = false;
    }
    oss << R"(])"
        << R"(,"bals_ex":[])"   // optional value for future
        << "}";
}

inline void parseTradeExecution(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": exec_id: " << document["exec_id"].GetString() << std::endl;
    std::cout << __func__ << ": ord_cl_id: " << document["ord_cl_id"].GetString() << std::endl;
    std::cout << __func__ << ": ord_ex_id: " << document["ord_ex_id"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << __func__ << ": amount: " << document["amount"].GetString() << std::endl;
    std::cout << __func__ << ": price: " << document["price"].GetString() << std::endl;
    std::cout << __func__ << ": liquidity: " << document["liquidity"].GetString() << std::endl;
    std::cout << __func__ << ": fee: " << document["fee"].GetString() << std::endl;
    std::cout << __func__ << ": fee_currency: " << document["fee_currency"].GetString() << std::endl;
    std::cout << __func__ << ": bals: " << std::endl;
    if (document.HasMember("bals") && document["bals"].IsArray()) {
        for (auto& item : document["bals"].GetArray()) {
            if (item.IsObject()) {
              for (auto& m : item.GetObject())
                std::cout << __func__ << "\t" << m.name.GetString() << ": " << m.value.GetString() << std::endl;
            }
        }
    }
}


/*** Order Done Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
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
inline void formatOrderDone(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& anOrdClId, 
        const std::string& anOrdExchId, 
        const std::string& aSymbol, 
        const std::string& aCancelId) {
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"ordn")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"ord_cl_id":")" << anOrdClId << R"(")"
        << R"(,"ord_ex_id":")" << anOrdExchId << R"(")"
        << R"(,"symbol":")"<< aSymbol << R"(")"
        << R"(,"can_id":")" << aCancelId << R"(")"
        << "}";
}

inline void parseOrderDone(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": ord_cl_id: " << document["ord_cl_id"].GetString() << std::endl;
    std::cout << __func__ << ": ord_ex_id: " << document["ord_ex_id"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << __func__ << ": can_id: " << document["can_id"].GetString() << std::endl;
}

/*** Order Reject Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
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
inline void formatOrderReject(std::ostringstream &oss,
        const uint64_t anId, 
        const std::string& anOrdClId, 
        const std::string& aSymbol, 
        const std::string& aReason, 
        const std::string& aRejCode) {
    oss << "{"
        << R"("id":")" << anId << R"(")"
        << R"(,"type":"orrj")"
        << R"(,"ts":")" << getMicrosSinceEpoch() << R"(")"
        << R"(,"ord_cl_id":")" << anOrdClId << R"(")"
        << R"(,"symbol":")"<< aSymbol << R"(")"
        << R"(,"reason":")" << aReason << R"(")"
        << R"(,"code":")" << aRejCode << R"(")"
        << "}";
}

inline void parseOrderReject(const std::string& json) {
    Document document;
    document.Parse(json.c_str());

    // values.
    std::cout << __func__ << ": id: " << document["id"].GetString() << std::endl;
    std::cout << __func__ << ": type: " << document["type"].GetString() << std::endl;
    std::cout << __func__ << ": ts: " << document["ts"].GetString() << std::endl;
    std::cout << __func__ << ": ord_cl_id: " << document["ord_cl_id"].GetString() << std::endl;
    std::cout << __func__ << ": symbol: " << document["symbol"].GetString() << std::endl;
    std::cout << __func__ << ": reason: " << document["reason"].GetString() << std::endl;
    std::cout << __func__ << ": code: " << document["code"].GetString() << std::endl;
}

/*** Cancel Reject Msg - server side
 * NOTE: number is encoded as string from server side as per MA2 protocol
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

    std::cout << "----------------------- Hello Msg -----------------------" << std::endl;
    formatHelloMsg(oss, 12345, "client0123", "BTX", "pasv", "true", "any");
    std::cout << "Json: " << oss.str() << std::endl;
    parseHelloMsg(oss.str());
    oss.str("");
    std::cout << "----------------------- HeartBeat Msg -----------------------" << std::endl;
    formatHeartbeat(oss, 12345);
    std::cout << "Json: " << oss.str() << std::endl;
    parseHeartbeat(oss.str());
    oss.str("");
    std::cout << "----------------------- Balance Request -----------------------" << std::endl;
    formatBalanceReq(oss, 12345);
    std::cout << "Json: " << oss.str() << std::endl;
    parseBalanceReq(oss.str());
    oss.str("");
    std::cout << "----------------------- Pair Data Request -----------------------" << std::endl;
    formatPairDataReq(oss, 12345, "BTC", "USD");
    std::cout << "Json: " << oss.str() << std::endl;
    parsePairDataReq(oss.str());
    oss.str("");
    std::cout << "----------------------- Open Order Request -----------------------" << std::endl;
    formatOpenOrdersReq(oss, 12345);
    std::cout << "Json: " << oss.str() << std::endl;
    parseOpenOrdersReq(oss.str());
    oss.str("");
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
    std::cout << "----------------------- Cancel Order Batch -----------------------" << std::endl;
    std::vector<CancelOrderData> cancelOrders;
    cancelOrders.emplace_back("cltOid0123", "", 1230);
    cancelOrders.emplace_back("", "exchOid0321", 3210);
    formatCancelOrderBatch(oss, 12345, cancelOrders);
    std::cout << "Json: " << oss.str() << std::endl;
    parseCancelOrderBatch(oss.str());
    oss.str("");
    std::cout << "----------------------- Acknowledgement Msg -----------------------" << std::endl;
    formatAckMsg(oss, 12345, 54321);
    std::cout << "Json: " << oss.str() << std::endl;
    parseAckMsg(oss.str());
    oss.str("");
    std::cout << "----------------------- Error Msg -----------------------" << std::endl;
    formatErrorMsg(oss, 12345, 54321, 11001, "Invalid exch id in hello");
    std::cout << "Json: " << oss.str() << std::endl;
    parseErrorMsg(oss.str());
    oss.str("");
    std::cout << "----------------------- Exchange Status -----------------------" << std::endl;
    formatExchangeStatus(oss, 12345, "exchId0123", 1, "OK");
    std::cout << "Json: " << oss.str() << std::endl;
    parseExchangeStatus(oss.str());
    oss.str("");
    std::cout << "----------------------- Balances Report -----------------------" << std::endl;
    std::unordered_map<std::string, std::string> balances;
    balances.emplace("BTC/USD", "0.0123");
    balances.emplace("ETH/USD", "10.012");
    formatBalancesReport(oss, 12345, "AcctId123", balances);
    std::cout << "Json: " << oss.str() << std::endl;
    parseBalancesReport(oss.str());
    oss.str("");
    std::cout << "----------------------- Pairs Data Report -----------------------" << std::endl;
    std::vector<PairData> ccyPairs;
    ccyPairs.emplace_back("BTC", "USD", "BTC/USD", 0.01, 2, 0.0001, 0.0001, 100, 0.0001, 100, 1.0, "NA");
    formatPairDataReport(oss, 12345, "exchId0123", ccyPairs);
    std::cout << "Json: " << oss.str() << std::endl;
    parsePairDataReport(oss.str());
    oss.str("");
    std::cout << "----------------------- Open Orders Report -----------------------" << std::endl;
    std::vector<OpenOrder> orders;
    orders.emplace_back("exchOid123", "clt123", "BTC/USD", "ACTIVE", 0.0002, 0.0001, 38500);
    formatOpenOrdersReport(oss, 12345, "exch012", 54321, orders.size(), orders);
    std::cout << "Json: " << oss.str() << std::endl;
    parseOpenOrdersReport(oss.str());
    oss.str("");
    std::cout << "----------------------- Order Created -----------------------" << std::endl;
    formatOrderCreated(oss, 12345, "CltOid123", "ExchOid123", "client0123", "BTC/USD", 0.0001, 38500.0);
    std::cout << "Json: " << oss.str() << std::endl;
    parseOrderCreated(oss.str());
    oss.str("");
    std::cout << "----------------------- Order Status -----------------------" << std::endl;
    formatOrderStatus(oss, 12345, "CltOid123", "ExchOid123", "ACTIVE", "BTC/USD", 0.0001, 38500.0, "Active");
    std::cout << "Json: " << oss.str() << std::endl;
    parseOrderStatus(oss.str());
    oss.str("");
    std::cout << "----------------------- Trade Execution -----------------------" << std::endl;
    // std::unordered_map<std::string, std::string> balances; // defined previously
    formatTradeExecution(oss, 12345, "execId123", "cltOid123", "exchOid123", "BTC/USD", 0.0001, 35090, "M", 0.0, "USD", balances);    
    std::cout << "Json: " << oss.str() << std::endl;
    parseTradeExecution(oss.str());
    oss.str("");
    std::cout << "----------------------- Order Done -----------------------" << std::endl;
    formatOrderDone(oss, 12345, "CltOid123", "ExchOid123", "BTC/USD", "CanOid123");
    std::cout << "Json: " << oss.str() << std::endl;
    parseOrderDone(oss.str());
    oss.str("");
    std::cout << "----------------------- Order Reject -----------------------" << std::endl;
    formatOrderReject(oss, 123456, "clt0123", "BTC/USD", "Reject reason", "50100");
    std::cout << "Json: " << oss.str() << std::endl;
    parseOrderReject(oss.str());
    oss.str("");
    std::cout << "----------------------- Cancel Reject -----------------------" << std::endl;
    formatCancelReject(oss, 123456, "clt0123", "exchOid1234", "Reject reason", "cancelId012", "50100");
    std::cout << "Json: " << oss.str() << std::endl;
    parseCancelReject(oss.str());
    oss.str("");
    return 0;
}
