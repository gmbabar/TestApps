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

#ifndef _EXCHCONTRACT_HPP
#define _EXCHCONTRACT_HPP

#include <iostream>
#include <string>
#include <stdexcept>
// #include <nebula/tr/Logger.hpp>
#include "Sym.hpp"
// #include <nebula/tml/lexical_cast.hpp>
// #include <nebula/io/NumParse.hpp>
// #include <nebula/tml/ParamType.hpp>

// Exchange contract mapping
struct ExchContract {
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
        // Tick size sanity
        if(tickSize == 0.) {
            std::stringstream ss;
            ss << bfcSym << '/' << exchSym << " tick size is 0!";
            throw std::invalid_argument(ss.str());
        }

        // Qty multiplier fix
        if(qtyMultiplier == 0)
            qtyMultiplier = 1;

        // Calculate qty precision
        {
            double scaledQtyMultiplier = qtyMultiplier;
            while(scaledQtyMultiplier > 1.) {
                ++qtyPrecision;
                scaledQtyMultiplier /= 10.;
            }
        }

        // Calculate price precision
        {
            double scaledTickSize = tickSize;
            while(scaledTickSize < 1.) {
                scaledTickSize *= 10.;
                ++pxPrecision;
            }
        }
    }

    // Safe constructor using named parameters
    ExchContract(
            const BfcSym &aBfcSym,
            const ExchSym &aExchSym,
            const SecurityID aSecurityID,
            const SecurityType &aSecurityType,
            const QtyMultiplier aQtyMultiplier,
            const ExchMultiplier aExchMultiplier,
            const RootSym &aRootSym,
            const IsInverse aIsInverse,
            const TickSize aTickSize)
        : ExchContract(
                aBfcSym, aExchSym, 
                aSecurityID, aSecurityType,
                aQtyMultiplier, aRootSym, aIsInverse, aTickSize) {
        // Qty multiplier fix
        exchMultiplier = aExchMultiplier;
        if(exchMultiplier == 0.)
            exchMultiplier = 1.;

        // Calculate exchange precision
        {
            double scaledExchMultiplier = exchMultiplier;
            while(scaledExchMultiplier > 1.) {
                ++exchPrecision;
                scaledExchMultiplier /= 10.;
            }
            
            while(scaledExchMultiplier < 1.) {
                --exchPrecision;
                scaledExchMultiplier *= 10.;
            }
        }
    }

    // NOTE: Please avoid using this constructor directly in the future. 
    //       Use the named parameter constructors for safety.
    [[deprecated]]
    ExchContract(
            const nebula::tr::Sym &aBfcSym,
            const std::string &aExchSym,
            const unsigned int aSecurityID,
            const std::string &aSecurityType,
            const unsigned long aQtyMultiplier,
            const std::string &aRootSym,
            const bool aIsInverse,
            const double aTickSize)
        : ExchContract(
                BfcSym(aBfcSym),
                ExchSym(aExchSym),
                SecurityID(aSecurityID),
                SecurityType(aSecurityType),
                QtyMultiplier(aQtyMultiplier),
                RootSym(aRootSym),
                IsInverse(aIsInverse),
                TickSize(aTickSize)) {
#if 0
#ifdef DEBUG
            std::cerr << "[ExchContract] Avoid using ExchContract constructor without named parameters. Function overloading (for ExchMultiplier) is not safe for an object this critical." << std::endl;
#endif
#endif
    }
    
    // NOTE: Please avoid using this constructor directly in the future. 
    //       Use the named parameter constructors for safety.
    [[deprecated]]
    ExchContract(
            const nebula::tr::Sym &aBfcSym,
            const std::string &aExchSym,
            const unsigned int aSecurityID,
            const std::string &aSecurityType,
            const unsigned long aQtyMultiplier,
            const double aExchMultiplier,
            const std::string &aRootSym,
            const bool aIsInverse,
            const double aTickSize)
        : ExchContract(
                BfcSym(aBfcSym),
                ExchSym(aExchSym),
                SecurityID(aSecurityID),
                SecurityType(aSecurityType),
                QtyMultiplier(aQtyMultiplier),
                ExchMultiplier(aExchMultiplier),
                RootSym(aRootSym),
                IsInverse(aIsInverse),
                TickSize(aTickSize)) {
#if 0
#ifdef DEBUG
            std::cerr << "[ExchContract] Avoid using ExchContract constructor without named parameters. Function overloading (for ExchMultiplier) is not safe for an object this critical." << std::endl;
#endif
#endif
    }

    // Round down an order qty to the precision accepted by the exchange
    unsigned long roundDownToQtyPrecision(const unsigned long aQty) const {
        if(qtyMultiplier == nebula::CryptoToNebulaMult) {
            const long qtyPrecisionModulo = pow(10, nebula::CryptoToNebulaDecimalShift - qtyPrecision + exchPrecision);
            const auto remainder = aQty % qtyPrecisionModulo;
            const auto qty = aQty - remainder;
            return std::max<long>(qty, 0);
        }
        else if(qtyMultiplier == 1) {
            if(exchPrecision == 0) {
                return aQty;
            }
            else {
                const long qtyPrecisionModulo = pow(10, -qtyPrecision + exchMultiplier);
                const auto remainder = aQty % qtyPrecisionModulo;
                const auto qty = aQty - remainder;
                return std::max<long>(qty, 0);
            }
        }
        else {
            std::stringstream ss;
            ss << "Unsupported qty multiplier " << qtyMultiplier 
               << " for " << exchSym << '!';
            throw std::invalid_argument(ss.str());
        }
    }

    // Format Nebula qty as exchange quantity
    std::string formatQty(const unsigned long aQty) const {
        if(qtyMultiplier > 1) {
            const auto qty = this->roundDownToQtyPrecision(aQty);
            std::stringstream ss;
            ss << std::fixed << std::setprecision(qtyPrecision) 
               << (static_cast<double>(qty)/qtyMultiplier)*exchMultiplier;
            return ss.str();
        }
        else {
            return NEBULA_TML_LEXICAL_CAST(std::string, aQty*exchMultiplier);
        }
    }
    // Parse exchange qty into Nebula qty
    unsigned long parseQty(const std::string &aQty) const {
        if(qtyMultiplier == 1) {
            return NEBULA_TML_LEXICAL_CAST(double, aQty)/exchMultiplier;
        }
        else if(qtyMultiplier == nebula::CryptoToNebulaMult) {
            return static_cast<double>(nebula::parseNebulaCryptoFromString(aQty)/exchMultiplier);
        }
        else {
            return boost::lexical_cast<double>(aQty)*qtyMultiplier/exchMultiplier;
        }
    }
    unsigned long parseQty(const nebula::data::ConstBufRange &aBufRange) const {
        if(qtyMultiplier == 1) {
            double qty;
            auto bufEnd = nebula::io::NumParse::as<double>(
                    aBufRange.buf, aBufRange.bufEnd, &qty);
            if(nebula_unlikely(bufEnd > aBufRange.bufEnd)) {
                // In rare cases, this occurs for scientific notations.
                if (memchr(aBufRange.buf, 'e', aBufRange.length()))
                    return boost::lexical_cast<double>(aBufRange.asStr())/exchMultiplier;
                std::stringstream ss;
                ss << "Failed to parse qty from buf range: " << aBufRange;
                throw std::invalid_argument(ss.str());
            }
            return qty/exchMultiplier;
        }
        else if(qtyMultiplier == nebula::CryptoToNebulaMult) {
            return static_cast<double>(nebula::parseNebulaCryptoFromString(aBufRange.buf, aBufRange.bufEnd)/exchMultiplier);
        }
        else {
            return boost::lexical_cast<double>(aBufRange.asStr())*qtyMultiplier/exchMultiplier;
        }
    }
    unsigned long parseQtyFromDouble(const double aQty) const {
        if(qtyMultiplier == 1) {
            return aQty/exchMultiplier;
        }
        else if(qtyMultiplier == nebula::CryptoToNebulaMult) {
            return nebula::convertCryptoToNebulaDouble(aQty)/exchMultiplier;
        }
        else {
            return aQty*qtyMultiplier/exchMultiplier;
        }
    }

    double parseNebulaToMd2(const unsigned long aQty) const {
        if(qtyMultiplier == 1) {
            return aQty * exchMultiplier;
        }
        else if(qtyMultiplier == nebula::CryptoToNebulaMult) {
            return nebula::convertNebulaToCrypto(aQty) * exchMultiplier;
        }
        else {
            return aQty/qtyMultiplier*exchMultiplier;
        }
    }

    unsigned long parseOrderCnt(const std::string &aQty) const {
        return NEBULA_TML_LEXICAL_CAST(unsigned long, aQty);
    }
    unsigned long parseOrderCnt(const nebula::data::ConstBufRange &aBufRange) const {
        unsigned long orderCnt;
        const auto bufEnd = nebula::io::NumParse::as<unsigned long>(
                aBufRange.buf, aBufRange.bufEnd, &orderCnt);
        if(nebula_unlikely(bufEnd > aBufRange.bufEnd))
            orderCnt = 1;
        return orderCnt;
    }

    // Format price as exchange price
    std::string formatPx(const double aPx) const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(pxPrecision) << aPx;
        return ss.str();
    }
    double parsePx(const std::string &aPx) const {
        return boost::lexical_cast<double>(aPx);
    }
    double parsePx(const nebula::data::ConstBufRange &aBufRange) const {
        double px;
        auto bufEnd = nebula::io::NumParse::as<double>(
                aBufRange.buf, aBufRange.bufEnd, &px);
        if(nebula_unlikely(bufEnd > aBufRange.bufEnd)) {
            // In rare cases, this occurs for scientific notations.
            if (memchr(aBufRange.buf, 'e', aBufRange.length())
                        || memchr(aBufRange.buf, 'E', aBufRange.length())) {
                return boost::lexical_cast<double>(aBufRange.asStr());
            }
            std::stringstream ss;
            ss << "Failed to parse price from buf range: " << aBufRange;
            throw std::invalid_argument(ss.str());
        }
        return px;
    }

    // String logging
    std::string asStr() const {
        std::stringstream ss;
        ss << "ExchContract[" << bfcSym
           << " exch_sym=" << exchSym
           << " security_id=" << securityID
           << " security_type=" << securityType
           << " qty_multiplier=" << qtyMultiplier
           << " exch_multiplier=" << exchMultiplier
           << " root_sym=" << rootSym
           << " is_inverse=" << (isInverse ? 'T' : 'F')
           << " tick_size=" << std::fixed << std::setprecision(9) << tickSize
           << " px_precision=" << pxPrecision 
           << " qty_precision=" << qtyPrecision 
           << " exch_precision=" << exchPrecision << ']';
        return ss.str();
    }
    friend std::ostream& operator<<(std::ostream &o, const ExchContract &aExchContract) {
        return o << aExchContract.asStr();
    }
    template<template <typename> class LogStmt, typename BufT>
    friend LogStmt<BufT> operator<<(LogStmt<BufT> aStmt, const ExchContract &aExchContract) {
        return aStmt << aExchContract.asStr();
    }
};

typedef std::vector<ExchContract> ExchContracts;

// Load exchange contracts from usp_GetInstanceProducts stored proc
ExchContracts loadExchContractsFromDB(
        const nebula::tr::logger_type &aLogger,
        const unsigned long aAppID,
        const std::string &aDbCreds,
        const std::string &aStoredProc);

ExchContracts loadPfExchContractsFromDB(
        const nebula::tr::logger_type &aLogger,
        const unsigned long aAppID,
        const std::string &aDbCreds,
        const std::string &aStoredProc);

// Release a substream
void releaseSubstream(
        const nebula::tr::logger_type &aLogger,
        const unsigned long aAppID,
        const std::string &aDbCreds,
        const std::string &aStoredProc);


#endif /* NEBULA_GW_EXCHCONTRACT_HPP */
