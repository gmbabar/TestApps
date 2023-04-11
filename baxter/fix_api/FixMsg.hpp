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

#ifndef FIXMSG_HPP
#define FIXMSG_HPP

#include <array>
#include <bitset>
#include <ostream>
#include <stdexcept>
#include <algorithm>
#include <nebula/common.hpp>
#include <nebula/data/BufRange.hpp>
#include <nebula/io/NumParse.hpp>
#include <nebula/opt/Likely.hpp>
#include <nebula/tr/PxSide.hpp>
#include "FixTag.hpp"
#include "FixMsgType.hpp"

// Structure to hold parsed FIX fields from a message buffer
struct NEBULA_DECL FixMsg {
    // Fill repeating group
    struct NEBULA_DECL Fill {
        nebula::data::ConstBufRange FillExecID;
        nebula::data::ConstBufRange FillPx;
        nebula::data::ConstBufRange FillQty;
        nebula::data::ConstBufRange FillTradeID;
        nebula::data::ConstBufRange FillTime;
        nebula::data::ConstBufRange FillLiquidityInd;
        nebula::data::ConstBufRange FeeRate;
        nebula::data::ConstBufRange Fee;
    };

    // Incremental refresh repeating group
    struct NEBULA_DECL MarketDataRefresh {
        int MDUpdateAction;     // must be first field: 0: new, 1: replace, 2: delete
        std::string Symbol;
        nebula::tr::PxSide MDEntryType;
        double MDEntryPx;
        double MDEntrySize;
        std::string MDEntryID;

        std::string asStr() const {
            std::stringstream ss;
            ss << ' ' << FixTag::MDUpdateAction << '=' << MDUpdateAction     // must be first field
               << ' ' << FixTag::Symbol << '=' << Symbol
               << ' ' << FixTag::MDEntryType << '=' << MDEntryType
               << ' ' << FixTag::MDEntryPx << '=' << MDEntryPx
               << ' ' << FixTag::MDEntrySize << '=' << MDEntrySize
               << ' ' << FixTag::MDEntryID << '=' << MDEntryID;
            return ss.str();
        }
    };

    // Set a field
    void field(const unsigned long aField, const char *const aBuf, const char *const aBufEnd) {
        switch (aField) {
        case FixTag::MDUpdateAction:    // Must be the first element in repeating group
            if (m_mdRefreshEntries.size() > 0 && m_mdEntryIndex < static_cast<int>(m_mdRefreshEntries.size()-1)) {
                m_mdRefreshEntries[++m_mdEntryIndex].MDUpdateAction = static_cast<int>(aBuf[0]) - '0';
            }
            else {
                std::cout << "FixMsg::field(): Invalid index: " << m_mdEntryIndex
                          << " or NoMDEntries: " << m_mdRefreshEntries.size() << std::endl;
                return;
            }
            break;
        case FixTag::MDEntryType:
            // Snapshots will have not have MDUpdateAction, so repeating group starts here.
            if (m_isMarketDataSnapshot &&
                m_mdRefreshEntries.size() > 0 &&
                m_mdEntryIndex < static_cast<int>(m_mdRefreshEntries.size()-1))
                ++m_mdEntryIndex;
            m_mdRefreshEntries[m_mdEntryIndex].MDEntryType = (aBuf[0] == '0' ? nebula::tr::PxSide::bid : nebula::tr::PxSide::ask);
            break;
        case FixTag::MDEntryPx:
            m_mdRefreshEntries[m_mdEntryIndex].MDEntryPx = boost::lexical_cast<double>(std::string(aBuf, aBufEnd-aBuf));
            break;
        case FixTag::MDEntrySize:
            m_mdRefreshEntries[m_mdEntryIndex].MDEntrySize = boost::lexical_cast<double>(std::string(aBuf, aBufEnd-aBuf));
            break;
        case FixTag::MDEntryID:
            m_mdRefreshEntries[m_mdEntryIndex].MDEntryID = std::string(aBuf, aBufEnd-aBuf);
            break;
        case FixTag::Symbol:
            if (m_mdEntryIndex >= 0 && m_mdEntryIndex < static_cast<int>(m_mdRefreshEntries.size())) {
                m_mdRefreshEntries[m_mdEntryIndex].Symbol = std::string(aBuf, aBufEnd-aBuf);
                break;
            }
            if (this->hasField(FixTag::MsgType) && m_fields[FixTag::MsgType][0] == FixMsgType::SecurityListResponse) {
                m_mdSecurityList.push_back(std::string(aBuf, aBufEnd-aBuf));
                break;
            }
        default:
            m_fields[aField] = nebula::data::ConstBufRange(aBuf, aBufEnd);
            m_available.set(aField);
            break;
        }
        // check if NoMDEntries is present and populated
        if(aField == FixTag::NoMDEntries) {
            m_mdRefreshEntries.resize(parseFixInt(m_fields[aField]));
        }
        else if (aField == FixTag::MsgType && aBuf[0] == FixMsgType::MarketDataSnapshot) {
            m_isMarketDataSnapshot = true;
        }
    }

    bool hasField(const unsigned long aField) const {
        return m_available.test(aField);
    }

    size_t mdEntryCount() const {
        return m_mdRefreshEntries.size();
    }

    size_t securityListCount() const {
        return m_mdSecurityList.size();
    }

    const std::string getSecurityListItem(size_t index) const {
        if (index < m_mdSecurityList.size())
            return { m_mdSecurityList[index] };
        return "";  // empty str;
    }

    const boost::optional<MarketDataRefresh> getMdEntry(size_t index) const {
        if (index < m_mdRefreshEntries.size())
            return { m_mdRefreshEntries[index] };
        return {};  // std::nullopt;
    }

    // Access fields
    const nebula::data::ConstBufRange& field(const unsigned long aField) const {
        if(nebula_unlikely(!m_available.test(aField))) {
            std::stringstream ss;
            ss << "Tried to access invalid fix field " << aField << ": " << asStr();
            throw std::invalid_argument(ss.str());
        }
        return m_fields[aField];
    }
    const nebula::data::ConstBufRange& operator[](const unsigned long aField) const {
        return field(aField);
    }

    // Reset available fields (for reusing message structure)
    void reset() {
        m_available.reset();
    }

    std::string asStr() const {
        std::stringstream ss;
        ss << "FixMsg[";
        bool first = true;
        for(std::size_t i = 0; i < m_available.size(); ++i) {
            if(!m_available[i])
                continue;
            if(!first)
                ss << ' ';
            else
                first = false;
            ss << i << '=' << m_fields[i].asStr();
        }
        for (std::size_t idx = 0; idx < m_mdRefreshEntries.size(); ++idx) {
            ss << ' ' << m_mdRefreshEntries[idx].asStr();
        }
        for (const auto symbol : m_mdSecurityList) {
            ss << ' ' << symbol;
        }
        ss << ']';
        return ss.str();
    }

    unsigned int parseFixInt(const nebula::data::ConstBufRange &aBufRange) const {
        if(nebula_unlikely(!aBufRange.valid()))
            throw std::invalid_argument("Failed to parse fix unsigned INT: Invalid buffer");
        const char *buf = aBufRange.buf;
        return nebula::io::NumParse::as<unsigned int>(&buf, aBufRange.bufEnd);
    }
    std::string fieldasStr() const {
        std::stringstream ss;
        ss << "FixMsg[";
        bool first = true;
        for(std::size_t i = 0; i < m_available.size(); ++i) {
            if(!m_available[i])
                continue;
            if(!first)
                ss << ' ';
            else
                first = false;
            ss << i << '=' << m_fields[i].asStr();
        }
        ss << ']';
        return ss.str();
    }

private:
    std::array<nebula::data::ConstBufRange, FixTag::MaxTagNum> m_fields;
    std::bitset<FixTag::MaxTagNum> m_available;

    int m_mdEntryIndex = -1;
    bool m_isMarketDataSnapshot = false;
    std::vector<MarketDataRefresh> m_mdRefreshEntries;
    std::vector<std::string> m_mdSecurityList;

};

// Parse a fix message buffer into a FixMsg object
inline bool parseFixMsg(const char *const aBuf, const char *const aBufEnd, FixMsg * const aMsgPtr) {
    const char *tag = nullptr;
    const char *tagEnd = nullptr;
    const char *val = nullptr;
    const char *valEnd = nullptr;
    const char *buf = aBuf;
    while (buf < aBufEnd) {
        if(tag == nullptr) {
            tag = buf;
            tagEnd = buf;
        }
        if(val == nullptr) {
            if(*buf != '=')
                tagEnd = buf + 1;
            else
                val = buf + 1;
        } else {
            if(*buf != '\x01') {
                valEnd = buf + 1;
            } else {
                if(nebula_unlikely(val == nullptr || valEnd == nullptr || tag == nullptr || tagEnd == nullptr)) {
                    std::cerr << "Failed to parse fix message: " << nebula::data::ConstBufRange(aBuf, aBufEnd) << std::endl;
                    std::cerr << "Partial parse: " << aMsgPtr->asStr() << std::endl;
                    return false;
                }
                if(tag != tagEnd) {
                    unsigned long field = nebula::io::NumParse::as<unsigned long>(&tag, tagEnd);
                    if(nebula_unlikely(field >= FixTag::MaxTagNum)) {
                        std::cout << "Failed to parse fix message: " << nebula::data::ConstBufRange(aBuf, aBufEnd) << std::endl;
                        std::cout << "Invalid fix tag: " << field << std::endl;
                        return false;
                    }
                    aMsgPtr->field(field, val, valEnd);
                }

                // Reset for next fields
                tag = nullptr;
                tagEnd = nullptr;
                val = nullptr;
                valEnd = nullptr;
            }
        }
        ++buf;
    }

    // Parse last field
    if(tag != nullptr && tagEnd != nullptr && val != nullptr && valEnd == nullptr) {
        unsigned long field = nebula::io::NumParse::as<unsigned long>(&tag, tagEnd);
        if(nebula_unlikely(field >= FixTag::MaxTagNum)) {
            std::cerr << "Failed to parse fix message: " << nebula::data::ConstBufRange(aBuf, aBufEnd) << std::endl;
            std::cerr << "Invalid final fix tag: " << field << std::endl;
            return false;
        }
        valEnd = aBufEnd;
        aMsgPtr->field(field, val, valEnd);
    }
    return true;
}

NEBULA_DECL inline std::ostream& operator<<(std::ostream &o, const FixMsg &aMsg) {
    return o << aMsg.asStr();
}

template<template<typename > class LogStmt, typename BufT>
inline LogStmt<BufT> operator<<(LogStmt<BufT> aStmt, const FixMsg &aMsg) {
    return aStmt << aMsg.asStr();
}

#endif /* FIXMSG_HPP */
