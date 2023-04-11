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

#ifndef FIXMSGPARSER_HPP
#define FIXMSGPARSER_HPP

#include <nebula/common.hpp>
#include <nebula/data/BufRange.hpp>
#include <nebula/dtm/DateParser.hpp>
#include <nebula/dtm/TimeParser.hpp>
#include <nebula/opt/Likely.hpp>
#include <nebula/opt/Prefault.hpp>
#include "nebula/tr/Logger.hpp"
#include "nebula/tr/ds/Delegates.hpp"
#include <string>
#include "DgtFixMsg.hpp"


struct NEBULA_DECL FixMsgParser {
    FixMsgParser(const nebula::tr::logger_type aLogger,
                 const nebula::tr::logger_type aMsgLogger,
                 const std::string &aLoggingName);

    // Subscribe
    void subscribeFixMsg(const DgtFixMsg &aDgt);

    // Parse
    bool process(const std::string &aMsg);
    bool process(const char * const aMsg, const char * const aMsgEnd);

private:
    nebula::tr::nlogger_type m_log;
    nebula::tr::logger_type m_mlog;
    nebula::tr::ds::Delegates<DgtFixMsg> m_dgtsFixMsg;
};

inline FixMsgParser::FixMsgParser(const nebula::tr::logger_type aLogger,
                                  const nebula::tr::logger_type aMsgLogger,
                                  const std::string &aLoggingName)
    : m_log(aLogger),
    m_mlog(aMsgLogger),
    m_dgtsFixMsg() {
    std::stringstream ss;
    ss << "[FixMsgParser:" << aLoggingName << ']';
    m_log.name(ss.str());
}

inline void FixMsgParser::subscribeFixMsg(const DgtFixMsg &aDgt) {
    m_dgtsFixMsg += aDgt;
}

inline bool FixMsgParser::process(const std::string &aMsg) {
    const char * buf = aMsg.c_str();
    const char * bufEnd = buf + aMsg.length();
    return this->process(buf, bufEnd);
}

inline bool FixMsgParser::process(const char * const aMsg, const char * const aMsgEnd) {
    // Log the message
    const nebula::data::ConstBufRange msgBuf(aMsg, aMsgEnd);
    m_mlog.inf() << "IN< " << msgBuf << m_mlog.endl();

    // Parse the FIX message buffer
    FixMsg fixMsg;
    if(parseFixMsg(aMsg, aMsgEnd, &fixMsg)) {
        m_dgtsFixMsg.broadcast(fixMsg);
        return true;
    }
    m_log.err() << "Failed to parse fix message: " << msgBuf << m_log.endl();
    return false;
}

#endif /* FIXMSGPARSER_HPP */
