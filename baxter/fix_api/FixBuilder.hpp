#ifndef FIXBUILDER_HPP
#define FIXBUILDER_HPP

#include <nebula/common.hpp>
#include <nebula/data/BufRange.hpp>
#include <nebula/dtm/DateTimeTypes.hpp>
#include <nebula/io/DynamicFmtBuf.hpp>
#include <nebula/mem/StackAlloc.hpp>
#include "FixTag.hpp"
#include "FmtBuf.hpp"
#include "common.hpp"
#include "DynamicFmtBuf.hpp"



// Builds a FIX message into a buffer
struct FixBuilder {
    static const char Sep = '\x01';

private:
    // dynamicFmtBuf allocate on the stack, stack size is 1100
    static const size_t StackSize = 1100;
    // space reserved in the front for message header
    static const size_t FrontOffset = 100;
    typedef StackAlloc<char, StackSize> Allocator;

public:
    FixBuilder(char * const aBuf, const char * const aBufEnd)
        : m_buf(aBuf, aBufEnd),
        m_fmtBuf(StackSize) {
        // leave 100 char in the front for the message header
        m_fmtBuf << nebula::io::Skip<FrontOffset>();
    }

    // Add a field to the body of the message
    void body(const FixTag aTag, const char aChar) {
        m_fmtBuf << static_cast<int>(aTag) << '=' << aChar << Sep;
    }
    void body(const FixTag aTag, const std::string &aStr) {
        m_fmtBuf << static_cast<int>(aTag) << '=' << aStr << Sep;
    }
    void body(const FixTag aTag, const double aDbl) {
        m_fmtBuf << static_cast<int>(aTag)
            << '=' << precision<8>(aDbl) << Sep;
    }
    void body(const FixTag aTag, const unsigned long anInt) {
        m_fmtBuf << static_cast<int>(aTag) << '=' << anInt << Sep;
    }
    void body(const FixTag aTag, const long anInt) {
        m_fmtBuf << static_cast<int>(aTag) << '=' << anInt << Sep;
    }
    void body(const int aTag, const long anInt) {
        m_fmtBuf << static_cast<int>(aTag) << '=' << anInt << Sep;
    }
    void body(const int aTag, const std::string &aStr) {
          m_fmtBuf << static_cast<int>(aTag) << '=' << aStr << Sep;
      }
    void body(const FixTag aTag, const nebula::dtm::ptime aTm,
              const bool aEnableMilliseconds=true) {
        const auto &date = aTm.date();
        const auto &dtm = aTm.time_of_day();
        m_fmtBuf << static_cast<int>(aTag) << '=';
        concatDate(date);
        m_fmtBuf << '-' << zeroPadField<2>(dtm.hours())
               << ':' << zeroPadField<2>(dtm.minutes())
               << ':' << zeroPadField<2>(dtm.seconds());
        if(aEnableMilliseconds)
            m_fmtBuf << '.' <<  zeroPadField<3>(dtm.total_milliseconds() % 1000);
        m_fmtBuf << Sep;
    }
private:
    template<typename Date>
    inline void concatDate(const Date& date) {
        m_fmtBuf << static_cast<unsigned short>(date.year())
               << zeroPadField<2>(static_cast<unsigned short>(date.month()))
               << zeroPadField<2>(static_cast<unsigned short>(date.day()));
    }
public:
    void body(const FixTag aTag, const boost::gregorian::date &aDate) {
        m_fmtBuf << static_cast<int>(aTag) << '=';
        concatDate(aDate);
        m_fmtBuf << Sep;
    }

    // Complete the message and return its buffer range
    nebula::data::ConstBufRange done(const char * const aFixVersionStr = "FIX.4.4") {
        // Get the body
        nebula::io::DynamicFmtBuf<Allocator> headerBuf(FrontOffset);
        headerBuf << "8=" << aFixVersionStr << Sep
                    << "9=" << m_fmtBuf.size() - FrontOffset << Sep;
        const auto adjustedOffset = FrontOffset - headerBuf.size();
        const auto msgBegin = m_fmtBuf.bufBegin + adjustedOffset;
        {
            const auto originalBegin = m_fmtBuf.bufBegin;
            m_fmtBuf.bufBegin = msgBegin;
            // prepend to m_fmtBuf
            memcpy(m_fmtBuf.bufBegin, headerBuf.bufBegin, headerBuf.size());
            // append check sum
            const auto checksum = generateCheckSum(m_fmtBuf.bufBegin, m_fmtBuf.size());
            m_fmtBuf << "10=" << zeroPadField<3>(checksum) << Sep;

            memcpy(m_buf.buf, m_fmtBuf.bufBegin, m_fmtBuf.size());
            // restore m_fmtBuf front pointer for correct dellocation
            m_fmtBuf.bufBegin = originalBegin;
        }
        return nebula::data::ConstBufRange(
                m_buf.buf, m_buf.buf + m_fmtBuf.size() - adjustedOffset);
    }

private:
    // From: https://www.onixs.biz/fix-dictionary/4.2/app_b.html
    unsigned int generateCheckSum(const char *buf, long bufLen) {
        unsigned int cks = 0;
        for(long idx = 0L; idx < bufLen; idx++)
             cks += (unsigned int)buf[idx];
        return cks % 256;
    }

    nebula::data::BufRange m_buf;
    DynamicFmtBuf<Allocator> m_fmtBuf;
};

#endif /* FIXBUILDER_HPP */
