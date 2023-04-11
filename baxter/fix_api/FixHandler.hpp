#ifndef FIXHANDLER_HPP
#define FIXHANDLER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "common.hpp"
#include <Sym.hpp>
// #include <nebula/dtm/HackTimer.hpp>
// #include <nebula/io/NumParse.hpp>
// #include <nebula/log/LogPtime.hpp>
// #include <nebula/opt/Likely.hpp>
// #include <nebula/tr/ds/DgtConnStatus.hpp>
// #include <nebula/tr/ds/EvtConnStatus.hpp>
#include <boost/unordered_map.hpp>
#include "FixBuilder.hpp"
#include "FixTag.hpp"
#include "FixMsg.hpp"
#include "FixMsgType.hpp"
#include "FixParsers.hpp"
#include "DgtFixMsg.hpp"

// Formatter helpers
namespace fix_helper {

typedef boost::unordered_map<Sym, nebula::gw::ExchContract> SymExchContractMap;

inline nebula::data::ConstBufRange fmtHeartbeat(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID,
        const std::string &aTestRequestID) {
    FixBuilder fix(aBuf, aBufEnd);
    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::Heartbeat));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    fix.body(FixTag::SendingTime, aTm);
    // Specific message
    if(!aTestRequestID.empty())
        fix.body(FixTag::TestReqID, aTestRequestID);
    return fix.done();
}
inline nebula::data::ConstBufRange fmtTestRequest(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID,
        const std::string &aTestRequestID) {
    FixBuilder fix(aBuf, aBufEnd);
    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::TestRequest));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    fix.body(FixTag::SendingTime, aTm);
    // Specific message
    fix.body(FixTag::TestReqID, aTestRequestID);
    return fix.done();
}
inline nebula::data::ConstBufRange fmtLogon(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID,
        const std::string &aUsername,
        const std::string &aPassword,
        const bool aCancelAllOnDisconnect) {

    FixBuilder fix(aBuf, aBufEnd);
    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::Logon));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    fix.body(FixTag::SendingTime, aTm);
    // Specific message
    fix.body(FixTag::EncryptMethod, "0");
    fix.body(FixTag::HeartBtInt, "30");
    fix.body(FixTag::Username, aUsername);
    fix.body(FixTag::Password, aPassword);
    fix.body(FixTag::ResetSeqNumFlag, "Y");
    fix.body(FixTag::CancelAllOnDisconnect, aCancelAllOnDisconnect ? "Y" : "N");
    return fix.done();
}
inline nebula::data::ConstBufRange fmtLogout(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID,
        const std::string &aText) {
    FixBuilder fix(aBuf, aBufEnd);
    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::Logout));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    fix.body(FixTag::SendingTime, aTm);
    // Specific message
    if(!aText.empty())
        fix.body(FixTag::Text, aText);
    return fix.done();
}
inline nebula::data::ConstBufRange fmtMarketDataRequest(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID,
        const std::string &anExchSym) {
    FixBuilder fix(aBuf, aBufEnd);
    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::MarketDataRequest));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    // Market Data Request
    fix.body(FixTag::MDReqID, aMsgSeqNum);
    fix.body(FixTag::SubscriptionRequestType, "1");
    fix.body(FixTag::MarketDepth, "0");
    fix.body(FixTag::NoMDEntryTypes, "1");
    fix.body(FixTag::MDEntryType, "0");
    // Baxter reject all requests even for a single unkown symbol/issue,
    // so we need to send subscriptions one by one
    fix.body(FixTag::NoRelatedSym, "1");
    fix.body(FixTag::Symbol, anExchSym);
    fix.body(FixTag::SendingTime, aTm);
    return fix.done();
}
inline nebula::data::ConstBufRange fmtSecurityListRequest(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID) {
    FixBuilder fix(aBuf, aBufEnd);
    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::SecurityListRequest));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    fix.body(FixTag::SendingTime, aTm);
    // Security Request
    fix.body(FixTag::SecurityReqID, aMsgSeqNum);
    fix.body(FixTag::SecurityListRequestType, "4");
    return fix.done();
}
inline nebula::data::ConstBufRange fmtNewOrder(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID,
        const nebula::gw::ExchContract &aExchContract,
        const nebula::tr::Order &aOrder) {
    // 8=FIX.4.2\0019=153\00135=D\00149=GLXY1\00156=BXTR\00157=TEST\00152=20210830-19:37:41.351\00134=3\00121=1\00111=7tc3gneo\00155=BTC/USD\00140=2\00138=0.0001\00144=66765\00154=2\00159=1\00118=6\00110=021\001
    FixBuilder fix(aBuf, aBufEnd);

    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::NewOrder));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    fix.body(FixTag::SendingTime, aTm);
    // Specific message
    fix.body(FixTag::ClOrdID, aOrder.id().asStr());
    fix.body(FixTag::Symbol, aExchContract.exchSym);
    fix.body(FixTag::Side, aOrder.side() == nebula::tr::Side::buy ? '1' : '2');
    fix.body(FixTag::OrderQty, aExchContract.formatQty(aOrder.qty()));
    // Only limit orders
    fix.body(FixTag::OrdType, "2");
    fix.body(FixTag::HandlInst, "1");
    fix.body(FixTag::Price, aExchContract.formatPx(aOrder.lmtPx()));
    // Only IOC is supported though
    fix.body(FixTag::TimeInForce, (aOrder.tif() == nebula::tr::TimeInForce::ioc ? '3' : '1'));
    if(aOrder.tif() == nebula::tr::TimeInForce::day_alo)
        fix.body(FixTag::ExecInst, '6');
    return fix.done();
}
inline nebula::data::ConstBufRange fmtSequenceReset(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID) {
    FixBuilder fix(aBuf, aBufEnd);
    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::SequenceReset));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    fix.body(FixTag::PossDupFlag, "Y");
    fix.body(FixTag::SendingTime, aTm);
    // Specific message
    fix.body(FixTag::GapFillFlag, "Y");
    std::stringstream ss;
    ss << aMsgSeqNum + 1;
    fix.body(FixTag::NewSeqNo, ss.str());

    return fix.done();
}
inline nebula::data::ConstBufRange fmtCancel(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID,
        const nebula::tr::Cancel &aCancel,
        const nebula::gw::SentOrder &aSentOrder) {
    FixBuilder fix(aBuf, aBufEnd);
    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::CancelRequest));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    fix.body(FixTag::SendingTime, aTm);
    // Specific message
    fix.body(FixTag::ClOrdID, aCancel.id().asStr()); // ID of the cancel
    if( !aSentOrder.exchID.empty())
        fix.body(FixTag::OrderID, aSentOrder.exchID);
    fix.body(FixTag::OrigClOrdID, aCancel.orderID().asStr()); // ID to cancel

    return fix.done();
}

inline nebula::data::ConstBufRange fmtCancelReplace(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &aTargetSubID,
        const nebula::tr::CancelReplace &aCancelReplace,
        const nebula::gw::SentOrder &aSentOrder) {
    FixBuilder fix(aBuf, aBufEnd);
    // Standard header
    fix.body(FixTag::MsgType, static_cast<char>(FixMsgType::CancelReplace));
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::TargetSubID, aTargetSubID);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    fix.body(FixTag::SendingTime, aTm);
    // Specific message
    //fix.body(FixTag::OrdType, '2'); Optional for switching order types
    fix.body(FixTag::OrigClOrdID, aCancelReplace.orderID().asStr()); // ID of previous
    if(!aSentOrder.exchID.empty())
        fix.body(FixTag::OrderID, aSentOrder.exchID); // Exch ID of previous (preferred)
    fix.body(FixTag::ClOrdID, aCancelReplace.id().asStr()); // ID of this cancelReplace
    fix.body(FixTag::OrderQty, aCancelReplace.qty()); // total intended qty, included already executed for this chain
    fix.body(FixTag::Price, aSentOrder.exchContractPtr->formatPx(aCancelReplace.lmtPx()));
    //fix.body(FixTag::StopPrice, '?'); STOP and Stop-Limit only
    //fix.body(FixTag::MinQty, '?'); ARCA only

    return fix.done();
}

/*
inline nebula::data::ConstBufRange fmt(
        char * const aBuf, const char *const aBufEnd,
        const nebula::dtm::ptime aTm,
        const unsigned long aMsgSeqNum,
        const std::string &aSenderCompID,
        const std::string &aTargetCompID,
        const std::string &orderID,
        const std::string &OrigClOrdID,
        bool aIncludeFills) {
    //  8=FIX.4.2\0019=99\00135=H\00149=elv9KO_8y0f8RM25B4kjKG882kqEsADl0JG31TKg\00156=BXTR\00152=20210831-03:44:34.811\00134=2\00137=*\00120000=Y\00110=108\001
    FixBuilder fix(aBuf, aBufEnd);
    fix.body(FixTag::MsgType, FixMsgType::OrderStatusRequest);
    fix.body(FixTag::SenderCompID, aSenderCompID);
    fix.body(FixTag::TargetCompID, aTargetCompID);
    fix.body(FixTag::SendingTime, aTm);
    fix.body(FixTag::MsgSeqNum, aMsgSeqNum);
    if(!orderID.empty())
        fix.body(FixTag::OrderID, orderID);
    if(!OrigClOrdID.empty())
        fix.body(FixTag::OrigClOrdID, OrigClOrdID);
    if(aIncludeFills)
        fix.body(20000, "Y");
    return fix.done();
}
*/

} // end fix_helper namespace

struct NEBULA_DECL FixHandler {
    //                                       Sender ID,   Target ID
    typedef nebula::func::FastDelegate<void(std::string, std::string)> DgtFixLogon;
    //                                       Sender ID,   Target ID,   Text
    typedef nebula::func::FastDelegate<void(std::string, std::string, std::string)> DgtFixLogout;
    //                                       TestReqID
    typedef nebula::func::FastDelegate<void(std::string)> DgtFixTestRequest;
    //
    typedef nebula::func::FastDelegate<void(const FixMsg&)> DgtFixReject;
    //
    typedef nebula::func::FastDelegate<void(const FixMsg&)> DgtFixBusinessMessageReject;
    //
    typedef nebula::func::FastDelegate<void()> DgtFixHeartbeatNeeded;
    //
    typedef nebula::func::FastDelegate<void()> DgtFixHeartbeatExpired;
    //
    typedef nebula::func::FastDelegate<void(std::string)> DgtFixHeartbeatReceived;
    //
    typedef nebula::func::FastDelegate<void(const FixMsg&)> DgtFixExecutionReport;
    //
    typedef nebula::func::FastDelegate<void(const FixMsg&)> DgtFixOrderCancelReject;
    //
    typedef nebula::func::FastDelegate<void(unsigned long, unsigned long)> DgtFixResendRequest;
    //
    typedef nebula::func::FastDelegate<void(const FixMsg&)> DgtFixMarketDataRefresh;
    //
    typedef nebula::func::FastDelegate<void(const FixMsg&)> DgtFixMarketDataSnapshot;
    //
    typedef nebula::func::FastDelegate<void(const FixMsg&)> DgtFixSecurityListResponse;

    template<typename SessionT>
    FixHandler(
            const nebula::tr::logger_type aLogger,
            SessionT * const aSessionPtr)
        : m_log(aLogger),
        m_timer(aSessionPtr->ios()),
        m_hbInterval(nebula::dtm::seconds(30)),
        m_lastMsgRecvTm(nebula::dtm::microsec_clock::local_time()),
        m_msgSeqNum(1),
        m_lastMsgSeqNum(0),
        m_ep("BXTRE") {
        m_log.name("[FixHandler]");

        // Subscribe to fix messages
        aSessionPtr->parser().subscribeFixMsg(dgtFixMsg());
        aSessionPtr->subscribeConnStatus(dgtConnStatus());
    }

    // Set/Get source
    void source(const std::string &aSource) {
        m_source = aSource;
    }
    const std::string& source() const {
        return m_source;
    }

    // Subscribe to parser  FIX
    void subscribeTestRequest(const DgtFixTestRequest &aDgt) {
        m_dgtsFixTestRequest += aDgt;
    }
    void subscribeHeartbeatNeeded(const DgtFixHeartbeatNeeded &aDgt) {
        m_dgtsFixHeartbeatNeeded += aDgt;
    }
    void subscribeHeartbeatExpired(const DgtFixHeartbeatExpired &aDgt) {
        m_dgtsFixHeartbeatExpired += aDgt;
    }
    void subscribeHeartbeatReceived(const DgtFixHeartbeatReceived &aDgt) {
        m_dgtsFixHeartbeatReceived += aDgt;
    }
    void subscribeReject(const DgtFixReject &aDgt) {
        m_dgtsFixReject += aDgt;
    }
    void subscribeBusinessMessageReject(const DgtFixBusinessMessageReject &aDgt) {
        m_dgtsFixBusinessMessageReject += aDgt;
    }
    void subscribeLogon(const DgtFixLogon &aDgt) {
        m_dgtsFixLogon += aDgt;
    }
    void subscribeResendRequest(const DgtFixResendRequest &aDgt) {
        m_dgtsFixResendRequest += aDgt;
    }
    void subscribeExecutionReport(const DgtFixExecutionReport &aDgt) {
        m_dgtFixExecutionReport += aDgt;
    }
    void subscribeOrderCancelReject(const DgtFixOrderCancelReject &aDgt) {
        m_dgtFixOrderCancelReject += aDgt;
    }
    void subscribeMarketDataRefresh(const DgtFixMarketDataRefresh &aDgt) {
        m_dgtFixMarketDataRefresh += aDgt;
    }
    void subscribeMarketDataSnapshot(const DgtFixMarketDataSnapshot &aDgt) {
        m_dgtFixMarketDataSnapshot += aDgt;
    }
    void subscribeSecurityListResponse(const DgtFixSecurityListResponse &aDgt) {
        m_dgtFixSecurityListResponse += aDgt;
    }

    void subscribeLogout(const DgtFixLogout &aDgt) {
        m_dgtsFixLogout += aDgt;
    }

/*
    // Format and send messages on  session
    template<typename SessionT, typename InstrT>
    void instruct(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const nebula::gw::ExchContract &aSymEp,
            const InstrT &anInstr) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);
        const auto msgRange = fix_helper::fmt(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                aTargetCompID, aSymEp, anInstr);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting: " << anInstr << m_log.endl();
    }
    */
    // Format and send messages on  session
    template<typename SessionT, typename CancelT, typename SentOrderT>
    void instructCancel(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID,
            const CancelT &aCancel,
            const SentOrderT &aSentOrder) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);
        const auto msgRange = fix_helper::fmtCancel(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                aTargetCompID, aTargetSubID, aCancel, aSentOrder);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting: " << aCancel << m_log.endl();
    }

    template<typename SessionT, typename CancelReplaceT, typename SentOrderT>
    void instructCancelReplace(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID,
            const CancelReplaceT &aCancelReplace,
            const SentOrderT &aSentOrder) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);
        const auto msgRange = fix_helper::fmtCancelReplace(range.buf, range.bufEnd,
            aUnivNow, nextMsgSeqNum(), aSenderCompID, aTargetCompID, aTargetSubID, aCancelReplace, aSentOrder);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting: " << aCancelReplace << m_log.endl();
    }

    template<typename SessionT>
    void instructSequenceReset(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID
            ) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);
        const auto msgRange = fix_helper::fmtSequenceReset(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(),
                aSenderCompID, aTargetCompID, aTargetSubID);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting seq reset" << m_log.endl();
    }

    // Format and send New Order
    template<typename SessionT, typename InstrT>
    void instructNewOrder(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID,
            const nebula::gw::ExchContract &aSymEp,
            const InstrT &anInstr) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);
        const auto msgRange = fix_helper::fmtNewOrder(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                aTargetCompID, aTargetSubID, aSymEp, anInstr);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting: " << anInstr << m_log.endl();
    }

    template<typename SessionT>
    void instructTestRequest(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID,
            const std::string &aTestRequestID) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);

        const auto msgRange = fix_helper::fmtTestRequest(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                aTargetCompID, aTargetSubID, aTestRequestID);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting test request" << m_log.endl();
    }
    template<typename SessionT>
    void instructHeartbeat(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID,
            const std::string &aTestRequestID) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);

        const auto msgRange = fix_helper::fmtHeartbeat(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                aTargetCompID, aTargetSubID, aTestRequestID);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting heartbeat" << m_log.endl();
    }
    template<typename SessionT>
    void instructLogon(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID,
            const std::string &aUsername,
            const std::string &aPassword,
            bool aCancelAllOnDisconnect) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);

        const auto msgRange = fix_helper::fmtLogon(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                aTargetCompID, aTargetSubID, aUsername, aPassword, aCancelAllOnDisconnect);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting logon" << m_log.endl();
    }
    template<typename SessionT>
    void instructLogout(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID,
            const std::string &aText) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);

        const auto msgRange = fix_helper::fmtLogout(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                aTargetCompID, aTargetSubID, aText);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting out" << m_log.endl();
    }
    template<typename SessionT>
    void instructMarketDataRequest(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID,
            const fix_helper::SymExchContractMap &aSymExchContractMap) {
        static const std::size_t InitialBufSz = 2048;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);


        for (const auto &m : aSymExchContractMap) {
            // fix.body(FixTag::Symbol, m.second.exchSym);
            const auto msgRange = fix_helper::fmtMarketDataRequest(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                    aTargetCompID, aTargetSubID, m.second.exchSym);
            if(nebula_likely(msgRange.valid())) {
                aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
            }
            else {
                m_log.ftl() << "Failed formatting market data request" << m_log.endl();
                break;
            }
        }
    }
    template<typename SessionT>
    void instructSecurityListRequest(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aTargetSubID) {
        static const std::size_t InitialBufSz = 2048;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);

        const auto msgRange = fix_helper::fmtSecurityListRequest(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                aTargetCompID, aTargetSubID);
        if(nebula_likely(msgRange.valid())) {
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        }
        else {
            m_log.ftl() << "Failed formatting market data request" << m_log.endl();
        }
    }
    /*
    template<typename SessionT>
    void instruct(
            SessionT * const aSessionPtr,
            const nebula::dtm::ptime aUnivNow,
            const std::string &aSenderCompID,
            const std::string &aTargetCompID,
            const std::string &aOrderID,
            const std::string &aOrigClOrdID,
            bool aIncludeFills) {
        static const std::size_t InitialBufSz = 1024;
        char buff[InitialBufSz];
        nebula::data::BufRange range(buff, InitialBufSz);

        const auto msgRange = fix_helper::fmt(range.buf, range.bufEnd, aUnivNow, nextMsgSeqNum(), aSenderCompID,
                aTargetCompID, aOrderID, aOrigClOrdID, aIncludeFills);
        if(nebula_likely(msgRange.valid()))
            aSessionPtr->socketSend(msgRange.buf, msgRange.bufEnd);
        else
            m_log.ftl() << "Failed formatting out" << m_log.endl();
    }
    */
    void setNextMsgSeqNum(unsigned long nextSeq) {
        m_msgSeqNum = nextSeq;
    }

private:
    nebula::tr::ds::DgtConnStatus dgtConnStatus() {
        return nebula::tr::ds::DgtConnStatus(this, &FixHandler::onConnStatus);
    }
    nebula::tr::ds::EvtOwnership onConnStatus(nebula::tr::ds::EvtConnStatus * const anEvtPtr) {
        m_log.inf() << "ConnStatus: " << *anEvtPtr << m_log.endl();
        if(anEvtPtr->status() == nebula::tr::ds::ConnStatus::disconnected) {
            boost::system::error_code ec;
            m_timer.cancel(ec);
        }
        return nebula::tr::ds::EvtOwnership::done;
    }

    void scheduleHeartbeatTest() {
        m_timer.expires_from_now(m_hbInterval);
        m_timer.async_wait(
                boost::bind(&FixHandler::onHeartbeatTest, this, nebula::asio::placeholders::error));
    }
    void onHeartbeatTest(const boost::system::error_code &ec) {
        if(!ec) {
            // Need to send our heartbeat
            m_dgtsFixHeartbeatNeeded.broadcast();

            // Send test request if needed
            const auto now = nebula::dtm::microsec_clock::local_time();
            if(now > m_lastMsgRecvTm + (2 * m_hbInterval)) {
                m_log.wrn() << "Heartbeat failure. Sending test request: " << now
                            << " > " << m_lastMsgRecvTm << " + " << m_hbInterval << m_log.endl();
                m_dgtsFixHeartbeatExpired.broadcast();
            }
            scheduleHeartbeatTest();
        }
    }

    // Callback for unparsed Baxter FIX messages
    DgtFixMsg dgtFixMsg() {
        return DgtFixMsg(this, &FixHandler::onFixMsg);
    }
    void onFixMsg(const FixMsg &aMsg) {
        try {
            this->parseFixMsg(aMsg);
        } catch (const std::exception &se) {
            m_log.err() << "Exception thrown when parsing fix message: " << se.what() << m_log.endl();
            std::cerr << "Exception thrown when parsing fix message: " << se.what() << std::endl;
        }
    }
    void parseFixMsg(const FixMsg &aMsg) {
        // Set the last msg received time
        m_lastMsgRecvTm = nebula::dtm::microsec_clock::local_time();

        // Check sequence numbers
        const unsigned long msgSequence = parseFixULong(aMsg[FixTag::MsgSeqNum]);
        if(m_lastMsgSeqNum != 0 && msgSequence != m_lastMsgSeqNum + 1)
            m_log.err() << "Detected FIX sequence gap: " << msgSequence << " != " << m_lastMsgSeqNum << " + 1";
        m_lastMsgSeqNum = msgSequence;

        // Handle different message types
        const auto &msgTypeBuf = aMsg[FixTag::MsgType];
        if(!msgTypeBuf.valid()) {
            m_log.err() << "Failed to find message type: " << aMsg << m_log.endl();
            return;
        }
        FixMsgType msgType = static_cast<FixMsgType>(msgTypeBuf.buf[0]);
        switch (msgType) {
        case FixMsgType::Heartbeat:
            {
                // Heartbeat received
                std::string testReqId = "";
                if (aMsg.hasField(FixTag::TestReqID)){
                    testReqId = aMsg[FixTag::TestReqID].asStr();
                }
                m_dgtsFixHeartbeatReceived.broadcast(testReqId);
                break;
            }
        case FixMsgType::TestRequest:
            // Test Request
            m_dgtsFixTestRequest.broadcast(aMsg[FixTag::TestReqID].asStr());
            break;
        case FixMsgType::ResendRequest:
        {
            m_log.inf() << "Received FIX Resend Request: " << aMsg << m_log.endl();
            m_dgtsFixResendRequest.broadcast(aMsg.parseFixInt(aMsg[FixTag::BeginSeqNo]), aMsg.parseFixInt(aMsg[FixTag::EndSeqNo]));
            break;
        }
        case FixMsgType::Reject:
        {
            m_log.inf() << "Received FIX Reject: " << aMsg << m_log.endl();
            m_dgtsFixReject.broadcast(aMsg);
            break;
        }
        case FixMsgType::Logon:
            if(msgTypeBuf.size() == 1) {
                m_log.inf() << "Received FIX Logon: " << aMsg << m_log.endl();
                if(aMsg.hasField(FixTag::HeartBtInt)) {
                    auto interval = parseFixULong(aMsg[FixTag::HeartBtInt]);
                    if (0 < interval && interval < 600)     // Baxter side implementation is fairly unstable
                        m_hbInterval = nebula::dtm::seconds(interval);
                    m_log.inf() << "Set FIX heartbeat interval to: " << m_hbInterval << m_log.endl();
                    scheduleHeartbeatTest();
                }
                m_dgtsFixLogon.broadcast(aMsg[FixTag::SenderCompID].asStr(),
                        aMsg[FixTag::TargetCompID].asStr());
            } else {
                m_log.err() << "Failed to handle FIX message type: " << aMsg << m_log.endl();
            }
            break;
        case FixMsgType::Logout:
            m_dgtsFixLogout.broadcast(aMsg[FixTag::SenderCompID].asStr(),
                    aMsg[FixTag::TargetCompID].asStr(),
                    aMsg.hasField(FixTag::Text) ? aMsg[FixTag::Text].asStr() : std::string());
            break;
        case FixMsgType::BusinessMessageReject:
        {
            m_log.inf() << "Received FIX BusinessMessageReject: " << aMsg << m_log.endl();
            m_dgtsFixBusinessMessageReject.broadcast(aMsg);
           break;
        }
        case FixMsgType::ExecutionReport:
            m_log.inf() << "Received FIX ExecutionReport: " << aMsg << m_log.endl();
            m_dgtFixExecutionReport.broadcast(aMsg);
            break;

        case FixMsgType::OrderCancelReject:
            m_log.inf() << "Received FIX OrderCancelReject: " << aMsg << m_log.endl();
            m_dgtFixOrderCancelReject.broadcast(aMsg);
            break;

        case FixMsgType::MarketDataRequestReject:
            m_log.err() << "Received FIX MarketDataRequestReject: " << aMsg << m_log.endl();
            break;

        case FixMsgType::MarketDataSnapshot:
            m_log.inf() << "Received FIX MarketDataSnapshot: " << aMsg << m_log.endl();
            m_dgtFixMarketDataSnapshot.broadcast(aMsg);
            break;

        case FixMsgType::MarketDataRefresh:
            // m_log.inf() << "Received FIX MarketDataRefresh: " << aMsg << m_log.endl();
            m_dgtFixMarketDataRefresh.broadcast(aMsg);
            break;

        case FixMsgType::SecurityListResponse:
            m_log.inf() << "Received FIX SecurityListResponse: " << aMsg << m_log.endl();
            m_dgtFixSecurityListResponse.broadcast(aMsg);
            break;

        default:
            m_log.err() << "Failed to handle FIX message type: " << aMsg << m_log.endl();
        }
    }

    unsigned long nextMsgSeqNum() {
        return m_msgSeqNum++;
    }

public:

private:
    nebula::tr::nlogger_type m_log;
    nebula::dtm::hacktime_timer m_timer;
    nebula::dtm::time_duration m_hbInterval;
    nebula::dtm::ptime m_lastMsgRecvTm;
    unsigned long m_msgSeqNum;
    unsigned long m_lastMsgSeqNum;
    nebula::tr::Ep m_ep;
    std::string m_source;

    nebula::tr::ds::Delegates<DgtFixTestRequest> m_dgtsFixTestRequest;
    nebula::tr::ds::Delegates<DgtFixHeartbeatNeeded> m_dgtsFixHeartbeatNeeded;
    nebula::tr::ds::Delegates<DgtFixHeartbeatExpired> m_dgtsFixHeartbeatExpired;
    nebula::tr::ds::Delegates<DgtFixHeartbeatReceived> m_dgtsFixHeartbeatReceived;
    nebula::tr::ds::Delegates<DgtFixLogon> m_dgtsFixLogon;
    nebula::tr::ds::Delegates<DgtFixReject> m_dgtsFixReject;
    nebula::tr::ds::Delegates<DgtFixBusinessMessageReject> m_dgtsFixBusinessMessageReject;
    nebula::tr::ds::Delegates<DgtFixResendRequest> m_dgtsFixResendRequest;
    nebula::tr::ds::Delegates<DgtFixLogout> m_dgtsFixLogout;
    nebula::tr::ds::Delegates<DgtFixExecutionReport> m_dgtFixExecutionReport;
    nebula::tr::ds::Delegates<DgtFixOrderCancelReject> m_dgtFixOrderCancelReject;
    nebula::tr::ds::Delegates<DgtFixMarketDataRefresh> m_dgtFixMarketDataRefresh;
    nebula::tr::ds::Delegates<DgtFixMarketDataSnapshot> m_dgtFixMarketDataSnapshot;
    nebula::tr::ds::Delegates<DgtFixSecurityListResponse> m_dgtFixSecurityListResponse;
};

#endif /* FIXHANDLER_HPP */
