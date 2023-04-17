

#include <cstddef>
#include <cassert>
#include <cstring>
#include <ostream>
#include <string>
#include <boost/functional/hash.hpp>

struct  BufRange {
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

 inline bool operator==(const BufRange &lhs, const BufRange &rhs) {
    return lhs.size() == rhs.size()
        && std::memcmp(lhs.buf, rhs.buf, lhs.size()) == 0;
}

 inline bool operator!=(const BufRange &lhs, const BufRange &rhs) {
    return lhs.size() != rhs.size()
        || std::memcmp(lhs.buf, rhs.buf, lhs.size()) != 0;
}

 std::ostream& operator<<(std::ostream &o, const BufRange &aBuf);

struct  ConstBufRange {
    typedef const char*  const_iterator;
    typedef std::size_t size_type;
    const char* buf;
    const char* bufEnd;

    inline size_type size() const {
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

    inline const char& operator[](std::size_t pos) const {
        assert(pos <= size());
        return *(buf+pos);
    }

    inline std::size_t find(char c, std::size_t pos=0) const {
        const std::size_t _size = size();
        if (pos >= _size)
            return std::string::npos;

        const char *_first = buf+pos;
        std::size_t _len = _size-pos;
        const char *f = (const char *) memchr(_first, c, _len);
        return (!f) ? std::string::npos : f-buf;
    }

    inline std::size_t _find(const char* s, std::size_t n, std::size_t pos) const {
        assert(n>0);
        assert(pos <= size());
        const char _elem0 = s[0];
        const char *_first = buf+pos;
        const char *_last = bufEnd;
        std::size_t _len = size()-pos;
        while (_len >= n) {
            _first = (const char*) memchr(_first, _elem0, _len-n+1);
            if (!_first)
                return std::string::npos;
            if (memcmp(_first, s, n) == 0)
                return _first-buf;
            _len = _last - ++_first;
        }
        return std::string::npos;
    }

    inline std::size_t find(const char* s, std::size_t pos=0) const {
        if (pos > size())
            return std::string::npos;

        const std::size_t _n = strlen(s);
        if (_n == 0)
            return pos;

        return _find(s, _n, pos);
    }

    inline std::size_t find(const std::string &s, std::size_t pos=0) const {
        if (pos > size())
            return std::string::npos;

        const std::size_t _n = s.length();
        if (_n == 0)
            return pos;

        return _find(s.c_str(), _n, pos);
    }

    inline std::size_t rfind(char c, std::size_t pos=std::string::npos) const {
        std::size_t _size = size();
        if (_size) {
            if (--_size > pos)
                _size = pos;
            for (++_size; _size-- > 0;)
                if (*(buf+_size) == c)
                    return _size;
        }
        return std::string::npos;
    }

    inline ConstBufRange substr(std::size_t pos=0, std::size_t n=std::string::npos) const {
        std::size_t _size = size();
        assert(pos <= _size);
        return ConstBufRange(buf+pos, n < _size-pos ? n : _size-pos);
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

struct ConstBufRangeMapHash : std::unary_function<ConstBufRange, std::size_t> {
    inline std::size_t operator()(const ConstBufRange &aBufRange) const {
        return boost::hash_range<ConstBufRange::const_iterator>(
                    aBufRange.begin(), aBufRange.end());
    }
};

struct ConstBufRangeMapPred {
    inline bool operator()(const ConstBufRange &aBufRange, const std::string &aStr) const {
        return (aBufRange.size() == aStr.length()) &&
               !(std::memcmp(aBufRange.begin(), aStr.c_str(), aStr.length()));
    }
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

