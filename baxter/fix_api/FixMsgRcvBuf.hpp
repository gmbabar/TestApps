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
#ifndef FIXMSGRCVBUF_HPP
#define FIXMSGRCVBUF_HPP

#include <nebula/common.hpp>
#include <nebula/data/BufRange.hpp>
#include "nebula/tr/Logger.hpp"
#include <cstddef>
#include <cstring>


template<typename ParserT>
struct NEBULA_DECL FixMsgRcvBuf {
    typedef ParserT  parser_type;

    FixMsgRcvBuf(const parser_type &aParser,
                  const nebula::tr::logger_type aLogger,
                  const std::size_t aBufSz = 1024*10)
        : m_parser(aParser),
        m_bufSz(aBufSz),
        m_buf(new char[m_bufSz]),
        m_bufBegin(m_buf),
        m_bufEnd(m_buf + m_bufSz) {}

    ~FixMsgRcvBuf() {
        delete [] m_buf;
    }

    parser_type& parser() {
        return m_parser;
    }
    const parser_type& parser() const {
        return m_parser;
    }

    char* begin() {
        return m_bufBegin;
    }
    const char* begin() const {
        return m_bufBegin;
    }
    const char* end() {
        return m_bufEnd;
    }

    const char* cbegin() const {
        return m_bufBegin;
    }
    const char* cend() const {
        return m_bufEnd;
    }

    const char *internal_begin() const {
        return m_buf;
    }

    std::size_t size() const {
        return m_bufEnd - m_bufBegin;
    }

    std::size_t maxSize() const {
        return m_bufSz;
    }

    void clear() {
        m_bufBegin = m_buf;
    }

    // void parse(const std::string &mesg) {
    //     m_parser.process(mesg);
    // }

    void parse(const std::size_t aBytesRead) {
        using nebula::data::ConstBufRange;

        static const char Needle[] = "\x01""10=";
        static std::size_t NeedleSize = 4;
        const char * const bufEnd = m_bufBegin + aBytesRead;
        const char * msgBegin = m_buf;
        while(msgBegin < bufEnd) {
            const char * msgEnd = (const char *) memmem(msgBegin, bufEnd - msgBegin, &Needle[0], NeedleSize);
            if(msgEnd == nullptr) {
                break;
            }

            msgEnd += NeedleSize + 4;
            if(msgEnd > bufEnd) {
                break;
            }

            m_parser.process(msgBegin, msgEnd);
            msgBegin = msgEnd ;
        }

        if (msgBegin < bufEnd) {
            const size_t left = bufEnd - msgBegin;
            std::memcpy(m_buf, msgBegin, left);
            m_bufBegin = m_buf + left;
        }
        else {
            m_bufBegin = m_buf;
        }

        // Reallocate
        if (this->size() == 0) {
            // Double the buffer size
            const std::size_t left = m_bufSz;
            m_bufSz *= 2;

            // Allocate new buffer
            char * const newBuf = new char[m_bufSz];

            // Copy existing data
            std::memcpy(newBuf, m_buf, left);

            // Delete old buffer
            delete [] m_buf;
            m_buf = newBuf;
            m_bufBegin = m_buf + left;
            m_bufEnd = m_buf + m_bufSz;
        }
    }

private:
    parser_type m_parser;
    std::size_t m_bufSz;
    char *m_buf;
    char *m_bufBegin;
    const char *m_bufEnd;
};


#endif /* NEBULA_TR_DS_IMPL_FIXMSGRCVBUF_HPP */
