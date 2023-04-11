/**_
 * BLUE FIRE CAPITAL, LLC CONFIDENTIAL
 * _____________________
 *
 *  Copyright © [2007] - [2014] Blue Fire Capital, LLC
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

#include "nebula/gw/ExchContract.hpp"

#include <SQLAPI.h>
#include <nebula/db/DbHelper.hpp>

namespace nebula::gw {

ExchContracts loadExchContractsFromDB(
        const nebula::tr::logger_type &aLogger,
        const unsigned long aAppID,
        const std::string &aDbCreds,
        const std::string &aStoredProc) {
    nebula::tr::nlogger_type nlog(aLogger);
    {
        std::stringstream ss;
        ss << "[loadExchContractsFromDB:" << aDbCreds << ']';
        nlog.name(ss.str());
    }

    try {
        const auto dbCreds = nebula::db::parseDbCredentialsFromStr(aDbCreds);

        // Initialize connection
        SAConnection dbConn;
        dbConn.Connect(
                aDbCreds.c_str(), 
                dbCreds.user.c_str(), 
                dbCreds.password.c_str(), 
                SA_ODBC_Client);
        if(!dbConn.isConnected()) {
            std::stringstream ss;
            ss << "Failed to connect to DB: " << aDbCreds;
            nlog.err() << ss.str() << nlog.endl();
            throw std::invalid_argument(ss.str());
        }

        // Format query
        std::stringstream ss;
        ss << "Securities.stream.usp_GetInstanceProducts 1, " << aAppID << ", 'test', 'wrk', 0, 1, 2";
        const std::string query = ss.str();
        nlog.inf() << "Query: " << query << nlog.endl();

        // Execute query
        SACommand dbCmd(&dbConn, query.c_str());
        dbCmd.Execute();

        // Get results
        ExchContracts ecs;
        while(dbCmd.FetchNext()) {
            nlog.inf() << "Fields: " << nebula::db::cmdFieldsAsString(dbCmd) << nlog.endl();
            const char * const bfcSym = dbCmd.Field("BFCSymbol").asString();
            const char * const exchSym = dbCmd.Field("exchSymbol").asString();
            const unsigned long securityID = dbCmd.Field("SecurityID").asLong();
            const char * const securityType = dbCmd.Field("Type").asString();
            const unsigned long qtyMultiplier = dbCmd.Field("Multiplier").asLong();
            const char * const rootSym = dbCmd.Field("rootSymbol").asString();
            const bool isInverse = dbCmd.Field("isInverse").asBool();
            const double tickSize = dbCmd.Field("tickSize").asDouble();
            ecs.emplace_back(
                    nebula::tr::Sym(bfcSym), exchSym,
                    securityID, securityType,
                    qtyMultiplier, rootSym,
                    isInverse, tickSize);
            nlog.inf() << ecs.back() << nlog.endl();
        }
        return ecs;
    }
    catch(const SAException &aEx) {
        std::stringstream ss;
        ss << "Exception thrown while trying to retrieve exchange contracts from DB: "
           << nebula::db::exceptionAsString(aEx)
           << " cred=" << aDbCreds;
        nlog.err() << ss.str() << nlog.endl();
        throw std::logic_error(ss.str());
    }
}

ExchContracts loadPfExchContractsFromDB(
        const nebula::tr::logger_type &aLogger,
        const unsigned long aAppID,
        const std::string &aDbCreds,
        const std::string &aStoredProc) {
    nebula::tr::nlogger_type nlog(aLogger);
    {
        std::stringstream ss;
        ss << "[loadPfExchContractsFromDB:" << aDbCreds << ']';
        nlog.name(ss.str());
    }
    try {
        const auto dbCreds = nebula::db::parseDbCredentialsFromStr(aDbCreds);

        // Initialize connection
        SAConnection dbConn;
        dbConn.Connect(
                aDbCreds.c_str(), 
                dbCreds.user.c_str(), 
                dbCreds.password.c_str(), 
                SA_ODBC_Client);
        if(!dbConn.isConnected()) {
            std::stringstream ss;
            ss << "Failed to connect to DB: " << aDbCreds;
            nlog.err() << ss.str() << nlog.endl();
            throw std::invalid_argument(ss.str());
        }

        // Format query
        std::stringstream ss;
        ss << "Securities.stream.usp_GetInstanceProducts 1, " << aAppID << ", 'test', 'wrk', 0, 1, 2";
        const std::string query = ss.str();
        nlog.inf() << "Query: " << query << nlog.endl();

        // Execute query
        SACommand dbCmd(&dbConn, query.c_str());
        dbCmd.Execute();

        // Get results
        ExchContracts ecs;
        while(dbCmd.FetchNext()) {
            nlog.inf() << "Fields: " << nebula::db::cmdFieldsAsString(dbCmd) << nlog.endl();
            const char * const bfcSym = dbCmd.Field("bfcSymbol").asString();
            const char * const exchSym = dbCmd.Field("exchSymbol").asString();
            const unsigned long securityID = dbCmd.Field("bfcSymbolID").asLong();
            const char * const securityType = dbCmd.Field("securityType").asString();
            const unsigned long qtyMultiplier = dbCmd.Field("contractMultiplier").asLong();
            const char * const rootSym = dbCmd.Field("exchSymbol").asString();
            //const double contractValue = dbCmd.Field("contractValue").asDouble();
            //const double exchMultiplier = isInverse ? 1. : 1./contractValue;
            const double exchMultiplier = 1.;
            const double tickSize = 0.00000001;
            nebula::gw::ExchContract ec(
                    nebula::tr::Sym(bfcSym), exchSym,
                    securityID, securityType,
                    qtyMultiplier, exchMultiplier, rootSym,
                    false, tickSize);
            ecs.emplace_back(ec);
            nlog.inf() << ecs.back() << nlog.endl();
        }
        return ecs;
    }
    catch(const SAException &aEx) {
        std::stringstream ss;
        ss << "Exception thrown while trying to retrieve exchange contracts from DB: "
           << nebula::db::exceptionAsString(aEx)
           << " cred=" << aDbCreds;
        nlog.err() << ss.str() << nlog.endl();
        throw std::logic_error(ss.str());
    }
}
void releaseSubstream(
        const nebula::tr::logger_type &aLogger,
        const unsigned long aAppID,
        const std::string &aDbCreds,
        const std::string &aStoredProc) {
    nebula::tr::nlogger_type nlog(aLogger);
    {
        std::stringstream ss;
        ss << "[releaseSubstream:" << aDbCreds << ']';
        nlog.name(ss.str());
    }

    try {
        const auto dbCreds = nebula::db::parseDbCredentialsFromStr(aDbCreds);

        // Initialize connection
        SAConnection dbConn;
        dbConn.Connect(
                aDbCreds.c_str(), 
                dbCreds.user.c_str(), 
                dbCreds.password.c_str(), 
                SA_ODBC_Client);
        if(!dbConn.isConnected()) {
            std::stringstream ss;
            ss << "Failed to connect to DB: " << aDbCreds;
            nlog.err() << ss.str() << nlog.endl();
            throw std::invalid_argument(ss.str());
        }

        // Format query
        std::stringstream ss;
        ss << "Securities.stream.usp_ReleaseSubStreamId 1, " << aAppID << ", 'test', 'wrk', 0, 1, 2";
        const std::string query = ss.str();
        nlog.inf() << "Query: " << query << nlog.endl();

        // Execute query
        SACommand dbCmd(&dbConn, query.c_str());
        dbCmd.Execute();
    }
    catch(const SAException &aEx) {
        std::stringstream ss;
        ss << "Exception thrown while trying to reload substream from DB: "
           << nebula::db::exceptionAsString(aEx)
           << " cred=" << aDbCreds;
        nlog.err() << ss.str() << nlog.endl();
        throw std::logic_error(ss.str());
    }
}

} // nebula::gw
