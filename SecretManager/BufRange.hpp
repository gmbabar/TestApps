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

#ifndef NEBULA_DATA_BUFRANGE_HPP
#define NEBULA_DATA_BUFRANGE_HPP

#include "common.hpp"
#include <cstddef>
#include <cstring>
#include <ostream>
#include <string>

namespace nebula { namespace data {

/*! \headerfile <nebula/data/BufRange.hpp>
  \brief A buffer range.
  A pointer pair defining a memory buffer range.
  Just as C++ iterators, the end pointer #bufEnd
  should always point to one past the last valid
  buffer element.  A range is invalid if the
  current buffer position pointer #buf is
  pointing past the buffer end pointer #bufEnd.

  \author Sebastian Hauer
  \ingroup utils 
 */
struct NEBULA_DECL BufRange {
    typedef const char*  const_iterator;
    typedef char*        iterator;

    typedef std::size_t size_type;
    char* buf;
    const char* bufEnd;

    size_type size() const {
        return bufEnd >= buf ? bufEnd - buf : 0;
    }
    bool valid() const {
        return bufEnd >= buf;
    }
    bool invalid() const {
        return bufEnd < buf;
    }

    const_iterator cbegin() const {
        return buf;
    }
    const_iterator cend() const {
        return bufEnd;
    }

    iterator begin() {
        return buf;
    }
    const_iterator begin() const {
        return buf;
    }
    const_iterator end() const {
        return bufEnd;
    }

    std::size_t length() const {
        return bufEnd - buf;
    }
    std::string asStr() const {
        return std::string(buf, length());
    }

    BufRange() : buf(const_cast<char*>("invalid")), bufEnd(0) {}
    BufRange(char * const aBuf, const char * const aBufEnd)
        : buf(aBuf),
          bufEnd(aBufEnd) {}
    BufRange(char * const aBuf, const size_type aLen)
        : buf(aBuf),
          bufEnd(aBuf + aLen) {}
};

/*! \headerfile <nebula/data/BufRange.hpp>
  \brief Equality comparison between two buffer ranges.
  \relates ConstBufRange
  \author Sebastian Hauer
  \ingroup utils 
 */
NEBULA_DECL inline bool operator==(const BufRange &lhs, const BufRange &rhs) {
    return lhs.size() == rhs.size()
        && std::memcmp(lhs.buf, rhs.buf, lhs.size()) == 0;
}

/*! \headerfile <nebula/data/BufRange.hpp>
  \brief Inequality comparison between two buffer ranges.
  \relates ConstBufRange
  \author Sebastian Hauer
  \ingroup utils 
 */
NEBULA_DECL inline bool operator!=(const BufRange &lhs, const BufRange &rhs) {
    return lhs.size() != rhs.size()
        || std::memcmp(lhs.buf, rhs.buf, lhs.size()) != 0;
}

/*! \headerfile <nebula/data/BufRange.hpp>
  \brief Debug output operator for a buffer range.
  \relates BufRange
  \author Sebastian Hauer
  \ingroup utils 
 */
NEBULA_DECL std::ostream& operator<<(std::ostream &o, const BufRange &aBuf);

/*! \headerfile <nebula/data/BufRange.hpp>
  \brief A const buffer range.
  A pointer pair defining a memory buffer range.
  Just as C++ iterators, the end pointer #bufEnd
  should always point to one past the last valid
  buffer element.  A range is invalid if the
  current buffer position pointer #buf is
  pointing past the buffer end pointer #bufEnd.

  \author Sebastian Hauer
  \ingroup utils 
 */
struct NEBULA_DECL ConstBufRange {
    typedef const char*  const_iterator;
    typedef std::size_t size_type;
    const char* buf;
    const char* bufEnd;

    size_type size() const {
        return bufEnd >= buf ? bufEnd - buf : 0;
    }
    bool valid() const {
        return bufEnd >= buf;
    }
    bool invalid() const {
        return bufEnd < buf;
    }

    const_iterator cbegin() const {
        return buf;
    }
    const_iterator cend() const {
        return bufEnd;
    }

    const_iterator begin() const {
        return buf;
    }
    const_iterator end() const {
        return bufEnd;
    }
    
    std::size_t length() const {
        return bufEnd - buf;
    }
    std::string asStr() const {
        return std::string(buf, length());
    }

    ConstBufRange() : buf("invalid"), bufEnd(0) {}
    ConstBufRange(const char * const aBuf, const char * const aBufEnd)
        : buf(aBuf),
        bufEnd(aBufEnd) {}
    ConstBufRange(const char * const aBuf, const size_type aLen)
        : buf(aBuf),
        bufEnd(aBuf + aLen) {}
    explicit ConstBufRange(const std::string &aStr)
        : buf(aStr.c_str()),
        bufEnd(aStr.c_str() + aStr.length()) {}
    ConstBufRange(const BufRange &aRange)
        : buf(aRange.buf),
        bufEnd(aRange.bufEnd) {}
};

/*! \headerfile <nebula/data/BufRange.hpp>
  \brief Equality comparison between two const buffer ranges.
  \relates ConstBufRange
  \author Sebastian Hauer
  \ingroup utils 
 */
NEBULA_DECL inline bool operator==(const ConstBufRange &lhs, const ConstBufRange &rhs) {
    return lhs.size() == rhs.size()
        && std::memcmp(lhs.buf, rhs.buf, lhs.size()) == 0;
}

/*! \headerfile <nebula/data/BufRange.hpp>
  \brief Inequality comparison between two const buffer ranges.
  \relates ConstBufRange
  \author Sebastian Hauer
  \ingroup utils 
 */
NEBULA_DECL inline bool operator!=(const ConstBufRange &lhs, const ConstBufRange &rhs) {
    return lhs.size() != rhs.size()
        || std::memcmp(lhs.buf, rhs.buf, lhs.size()) != 0;
}

/*! \headerfile <nebula/data/BufRange.hpp>
  \brief Debug output operator for a const buffer range.
  \relates ConstBufRange
  \author Sebastian Hauer
  \ingroup utils 
 */
NEBULA_DECL std::ostream& operator<<(std::ostream &o, const ConstBufRange &aBuf);

}}

#endif /* NEBULA_DATA_BUFRANGE_HPP */
