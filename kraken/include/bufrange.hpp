#include <cstddef>
#include <cstring>
#include <ostream>
#include <string>
#include <stdlib.h>


namespace parser { namespace data {


struct BufRange {
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
    double asDouble() const {
        return atof(buf);
    }
    int asInt() const {
        return atoi(buf);
    }
    
    // inline std::size_t find(char c, std::size_t pos=0) {
    //     const std::size_t _size = size();
    //     if(pos >= _size)
    //         return std::string::npos;
    //     const char*_first = buf+pos;
    //     std::size_t _len = _size-pos;
    //     const char *f = (const char*) memchar(_first,len);
    //     return (!f) ? std::string::npos : f-buf; 
    // } 

    BufRange() : buf(const_cast<char*>("invalid")), bufEnd(0) {}
    BufRange(char * const aBuf, const char * const aBufEnd)
        : buf(aBuf),
          bufEnd(aBufEnd) {}
    BufRange(char * const aBuf, const size_type aLen)
        : buf(aBuf),
          bufEnd(aBuf + aLen) {}
};


inline bool operator==(const BufRange &lhs, const BufRange &rhs) {
    return lhs.size() == rhs.size()
        && std::memcmp(lhs.buf, rhs.buf, lhs.size()) == 0;
}

inline bool operator!=(const BufRange &lhs, const BufRange &rhs) {
    return lhs.size() != rhs.size()
        || std::memcmp(lhs.buf, rhs.buf, lhs.size()) != 0;
}

std::ostream& operator<<(std::ostream &o, const BufRange &aBuf);

struct ConstBufRange {
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


inline bool operator==(const ConstBufRange &lhs, const ConstBufRange &rhs) {
    return lhs.size() == rhs.size()
        && std::memcmp(lhs.buf, rhs.buf, lhs.size()) == 0;
}


inline bool operator!=(const ConstBufRange &lhs, const ConstBufRange &rhs) {
    return lhs.size() != rhs.size()
        || std::memcmp(lhs.buf, rhs.buf, lhs.size()) != 0;
}

std::ostream& operator<<(std::ostream &o, const ConstBufRange &aBuf);

}}
