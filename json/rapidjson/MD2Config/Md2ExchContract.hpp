/**_
 * BLUE FIRE CAPITAL, LLC CONFIDENTIAL
 * _____________________
 * 
 *  Copyright © [2007] - [2012] Blue Fire Capital, LLC
 *  All Rights Reserved.
 * 
 * NOTICE:  All information contained herein is, and remains the property
 * of Blue Fire Capital, LLC and its suppliers, if any.  The intellectual
 * and technical concepts contained herein are proprietary to Blue Fire
 * Capital, LLC and its suppliers and may be covered by U.S. and Foreign
 * Patents, patents in process, and are protected by trade secret or copyright
 * law.  Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained from 
 * Blue Fire Capital, LLC.
_**/

#ifndef NEBULA_GW_MD2EXCHCONTRACT_HPP
#define NEBULA_GW_MD2EXCHCONTRACT_HPP

#include <iostream>
#include <string>
// #include <stdexcept>
#include <sstream>
#include <boost/asio/ip/tcp.hpp>
#include <rapidjson/document.h>

struct ExchContract {
    typedef std::string Sym;
    Sym bfcSym;
    std::string exchSym;
    unsigned int securityID{0};
    std::string securityType;
    unsigned long qtyMultiplier{1};
    double exchMultiplier{1.};
    std::string rootSym;
    bool isInverse{false};
    double tickSize{0.000000001};
    unsigned long orderLotSize{0};
    unsigned long qtyPrecision{0};
    long exchPrecision{0};
    unsigned long pxPrecision{0};
    
    // Named parameters used by safer constructors
    typedef Sym BfcSym; 
    typedef std::string ExchSym; 
    typedef unsigned int SecurityID;
    typedef std::string SecurityType;
    typedef unsigned long QtyMultiplier;
    typedef double ExchMultiplier;
    typedef std::string RootSym;
    typedef bool IsInverse;
    typedef double TickSize;

    // Safe constructor using named parameters
    ExchContract(
            const BfcSym &aBfcSym,
            const ExchSym &aExchSym,
            const SecurityID aSecurityID,
            const SecurityType &aSecurityType,
            const QtyMultiplier aQtyMultiplier,
            const RootSym &aRootSym,
            const IsInverse aIsInverse,
            const TickSize aTickSize)
        : bfcSym(aBfcSym),
        exchSym(aExchSym),
        securityID(aSecurityID),
        securityType(aSecurityType),
        qtyMultiplier(aQtyMultiplier),
        exchMultiplier(1.),
        rootSym(aRootSym),
        isInverse(aIsInverse),
        tickSize(aTickSize),
        qtyPrecision(0),
        exchPrecision(0),
        pxPrecision(0) {
    }
};

typedef std::vector<ExchContract> ExchContracts;

// parse json web page and extract needed information.
// TODO: Add checks for bad format(s) - currently only rapidjson parser is going to reject.
inline bool getWebPageBody(const std::string& aHost, const std::string& aPort, const std::string& aPath, std::string &aBody) {
    bool body = false;
    boost::asio::ip::tcp::iostream stm(aHost, aPort);
    if(!stm) {
        std::cout << "Could not connect to " << aHost << "\n";
        return body;
    }
    stm << "GET " << aPath << " HTTP/1.0\r\n"
      << "Host: " << aHost << " \r\n"
      << "Accept: */*\r\n"
      << "Connection: close\r\n\r\n" ;
    std::ostringstream oss;
    for(std::string line; getline(stm, line); ) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.size() == 0)
            body = true;
        else if (body) {
            oss << line;
        }
    }
    aBody = oss.str();
    return body;
}

// pulls exchange id from sec master web api
int getExchangeId(const std::string& aHost, const std::string& aPort, const std::string& aPath, const std::string& aCode) {
    const int failure = -1;
    std::string jsonStr;
    if (!getWebPageBody(aHost, aPort, aPath, jsonStr))
        return failure;
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    if (!doc.IsObject() && !doc.IsArray()) {
        std::cout << "Failed to parse json string " << std::endl;
        return failure;
    }
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject() && item.HasMember("code") && item.HasMember("id")) {
                if (aCode == item["code"].GetString()) {
                    return item["id"].GetInt();
                }
            }
        }
    }
    return failure;
}

// exchange symbols.
bool getExchangeSymbols(const std::string& aHost, const std::string& aPort, const std::string& aPath, ExchContracts &aExchContracts) {
    std::string jsonStr;
    if (!getWebPageBody(aHost, aPort, aPath, jsonStr))
        return false;
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    if (!doc.IsObject() && !doc.IsArray()) {
        std::cout << "Failed to parse json string " << std::endl;
        return false;
    }
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject() && item.HasMember("externalSymbol") && item.HasMember("internalSymbol")) {
                aExchContracts.emplace_back(ExchContract(
                    ExchContract::Sym(item["internalSymbol"].GetString()),        // BfcSym
                    item["externalSymbol"].GetString(),             // ExchSym
                    (item.HasMember("securityId") && item["securityId"].GetInt64() ? item["securityId"].GetInt64() : 0), 
                    "CRYPTO", 
                    1.0, 
                    item["internalSymbol"].GetString(), 
                    (item.HasMember("isInverse") && item["isInverse"].IsBool() ? item["isInverse"].GetBool() : false), 
                    (item.HasMember("tickSize") && item["tickSize"].IsDouble() ? item["tickSize"].GetDouble() : 1.0)
                    )); 
            }
        }
    }
    return aExchContracts.size() > 0;
}

// typedef std::vector<ExchContract> ExchContracts;

// Load exchange contracts from sec master web services.
bool loadExchContractsFromSecMaster(
        const std::string &anExchCode,
        const std::string &aSecMasterHost,
        const std::string &aSecMasterPort,
        const std::string &aSecMasterPath,
        const std::string &aSymbolsHost,
        const std::string &aSymbolsPort,
        const std::string &aSymbolsPath,
        ExchContracts &exchContracts) {
    const int exchId = getExchangeId(aSecMasterHost, aSecMasterPort, aSecMasterPath, anExchCode);
    if (exchId < 0)
        return false;
    
    std::ostringstream oss;
    oss << aSymbolsPath << 1;
    return getExchangeSymbols(aSymbolsHost, aSymbolsPort, oss.str().c_str(), exchContracts);
}


#endif /* NEBULA_GW_MD2EXCHCONTRACT_HPP */
