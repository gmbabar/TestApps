#ifndef _IO_FMTBUF_HPP
#define _IO_FMTBUF_HPP

#include <boost/concept_check.hpp>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <string>
#include "common.hpp"


namespace nebula { namespace io {

struct FmtBuf {
    typedef std::size_t size_type;

    char* bufBegin;
    char* buf;
    const char* bufEnd;

    inline size_type size() const {
        return buf >= bufBegin ? buf - bufBegin : 0;
    }

    inline FmtBuf& fail() {
        buf += (bufEnd - bufBegin) + 1;
        return *this;
    }

    inline bool failed() const {
        return buf > bufEnd;
    }

    inline bool done() const {
        return buf >= bufEnd;
    }

    inline bool good() const {
        return buf < bufEnd;
    }

    inline std::string asString() {
        return std::string(bufBegin, this->size());
    }

    FmtBuf(char * const aBuf, const char * const aBufEnd)
        : bufBegin(aBuf),
          buf(aBuf),
          bufEnd(aBufEnd) {}
};

inline std::ostream& operator<<(std::ostream &o, const FmtBuf &aBuf) {
    return o.write(aBuf.bufBegin, aBuf.size());
}

inline bool operator==(const FmtBuf &lhs, const FmtBuf &rhs) {
    return lhs.good() && rhs.good() && lhs.size() == rhs.size()
        && std::strncmp(lhs.bufBegin, rhs.bufBegin, lhs.size()) == 0;
}

inline bool operator!=(const FmtBuf &lhs, const FmtBuf &rhs) {
    return ! operator==(lhs, rhs);
}

inline bool operator==(const FmtBuf &lhs, const char * const aCStr) {
    return lhs.good() && lhs.size() == strlen(aCStr) 
        && std::strncmp(lhs.bufBegin, aCStr, lhs.size()) == 0;
}

inline bool operator!=(const FmtBuf &lhs, const char * const aCStr) {
    return ! operator==(lhs, aCStr);
}

inline bool operator==(const char * const aCStr, const FmtBuf &rhs) {
    return operator==(rhs, aCStr);
}

inline bool operator!=(const char * const aCStr, const FmtBuf &rhs) {
    return ! operator==(rhs, aCStr);
}

struct FmtBufDump {
    const FmtBuf &buf;
    explicit FmtBufDump(const FmtBuf &aBuf)
        : buf(aBuf) {}
};
inline std::ostream& operator<<(std::ostream &o, const FmtBufDump &aBufDump) {
    o << "FmtBufDump[";
    if (aBufDump.buf.good()) {
        o << "good";
    } else if (aBufDump.buf.failed()) {
        o << "failed";
    } else if (aBufDump.buf.done()) {
        o << "done";
    } else {
        o << "bad";
    }
    return o << ", size: " << aBufDump.buf.size()
             << ", bufBegin: " << (void*) aBufDump.buf.bufBegin
             << ", buf: " << (void*) aBufDump.buf.buf
             << ", bufEnd: " << (void*) aBufDump.buf.bufEnd
             << ']';
}


template<unsigned int N>
FmtBuf& operator<<(FmtBuf &aBuf, const Skip<N>&) {
    if (nebula_likely(aBuf.buf < aBuf.bufEnd)) {
        aBuf.buf += Skip<N>::Count;
    }
    return aBuf;
}

inline FmtBuf& operator<<(FmtBuf &aBuf, const nebula::data::ConstBufRange &aRange) {
    if (nebula_likely(aBuf.good())) {
        const nebula::data::ConstBufRange::size_type sz = aRange.size();
        if (aBuf.buf + sz <= aBuf.bufEnd) {
            std::memcpy(aBuf.buf, aRange.buf, sz);
        }
        aBuf.buf += sz;
    }
    return aBuf;
}

inline FmtBuf& operator<<(FmtBuf &aBuf, const nebula::data::BufRange &aRange) {
    if (nebula_likely(aBuf.good())) {
        const nebula::data::BufRange::size_type sz = aRange.size();
        if (aBuf.buf + sz <= aBuf.bufEnd) {
            std::memcpy(aBuf.buf, aRange.buf, sz);
        }
        aBuf.buf += sz;
    }
    return aBuf;
}

inline FmtBuf& operator<<(FmtBuf &aBuf, const char * const aCStr) {
    if (nebula_likely(aBuf.good())) {
        const std::size_t sz = std::strlen(aCStr);
        if (aBuf.buf + sz <= aBuf.bufEnd) {
            std::memcpy(aBuf.buf, aCStr, sz);
        }
        aBuf.buf += sz;
    }
    return aBuf;
}

inline FmtBuf& operator<<(FmtBuf &aBuf, char * const aCStr) {
    if (nebula_likely(aBuf.good())) {
        const std::size_t sz = std::strlen(aCStr);
        if (aBuf.buf + sz <= aBuf.bufEnd) {
            std::memcpy(aBuf.buf, aCStr, sz);
        }
        aBuf.buf += sz;
    }
    return aBuf;
}

template<unsigned int N>
inline FmtBuf& operator<<(FmtBuf &aBuf, const char (&aChArr)[N]) {
    if (nebula_likely(aBuf.good() && N > 0)) {
        const unsigned int sz = N - 1;
        if (aBuf.buf + sz <= aBuf.bufEnd) {
            std::memcpy(aBuf.buf, aChArr, sz);
        }
        aBuf.buf += sz;
    }
    return aBuf;
}

inline FmtBuf& operator<<(FmtBuf &aBuf, const unsigned char aCh) {
    if (nebula_likely(aBuf.good())) {
        *(aBuf.buf++) = aCh;
    }
    return aBuf;
}

inline FmtBuf& operator<<(FmtBuf &aBuf, const char aCh) {
    if (nebula_likely(aBuf.good())) {
        *(aBuf.buf++) = aCh;
    }
    return aBuf;
}


template<unsigned int Width, char Padding, typename T>
struct PadNum {
    T val;
    PadNum(const T aVal)
        : val(aVal) {}
};

template<unsigned int Width, char Padding = ' ', typename T>
PadNum<Width, Padding, T> padNum(const T aVal) {
    return PadNum<Width, Padding, T>(aVal);
}

template<unsigned int Width, char Padding, typename T>
FmtBuf& operator<<(FmtBuf &aBuf, PadNum<Width, Padding, T> aPadNum) {
    aBuf.buf = nebula::io::NumPadFmt<Width, Padding>::of(aBuf.buf, aBuf.bufEnd, aPadNum.val);
    return aBuf;
}

template<typename T>
FmtBuf& operator<<(FmtBuf &aBuf, T aVal) {
    BOOST_CONCEPT_ASSERT((boost::Integer<T>));
    aBuf.buf = NumFmt::of(aBuf.buf, aBuf.bufEnd, aVal);
    return aBuf;
}

template<typename T>
struct VarPrecision {
    T value;
    unsigned int prec;
    VarPrecision(const T aVal, const unsigned int aPrec)
        : value(aVal),
          prec(aPrec) {}
};

template<typename T>
VarPrecision<T> varPrecision(const T aVal, const unsigned int aPrec) {
    return VarPrecision<T>(aVal, aPrec);
}

template<typename T>
FmtBuf& operator<<(FmtBuf &aBuf, VarPrecision<T> aPrecVal) {
    aBuf.buf = NumFmt::of(aBuf.buf, aBuf.bufEnd, aPrecVal.value, aPrecVal.prec);
    return aBuf;
}

template<typename T, unsigned int N>
struct Precision {
    static const unsigned int Prec = N;
    T value;
    explicit Precision(const T aVal)
        : value(aVal) {}
};

template<unsigned int N, typename T>
Precision<T, N> precision(const T aVal) {
    return Precision<T, N>(aVal);
}

template<typename T, unsigned int N> 
FmtBuf& operator<<(FmtBuf &aBuf, Precision<T, N> aPrecVal) {
    aBuf.buf = NumFmt::of(aBuf.buf, aBuf.bufEnd, aPrecVal.value, Precision<T, N>::Prec);
    return aBuf;
}

template<typename T, unsigned int WidthT, char PaddingT = '0'>
struct Pad {
    static const unsigned int Width = WidthT;
    static const unsigned int Padding = PaddingT;
    T value;
    explicit Pad(const T aVal) : value(aVal) {}
};

template<unsigned int WidthT, typename T>
Pad<T, WidthT, '0'> zeroPad(const T aVal) {
    return Pad<T, WidthT, '0'>(aVal);
}

template<typename T, unsigned int WidthT, char PaddingT> 
FmtBuf& operator<<(FmtBuf &aBuf, Pad<T, WidthT, PaddingT> aPad) {
    aBuf.buf = NumPadFmt<WidthT, PaddingT>::of(aBuf.buf, aBuf.bufEnd, aPad.value);
    return aBuf;
}

template<typename T, unsigned int WidthT, char PaddingT = '0'>
struct Field {
    static const unsigned int Width = WidthT;
    static const unsigned int Padding = PaddingT;
    T value;
    explicit Field(const T aVal) : value(aVal) {}
};

template<unsigned int WidthT, typename T>
Field<T, WidthT, '0'> zeroPadField(const T aVal) {
    return Field<T, WidthT, '0'>(aVal);
}

template<typename T, unsigned int WidthT, char PaddingT> 
FmtBuf& operator<<(FmtBuf &aBuf, Field<T, WidthT, PaddingT> aField) {
    aBuf.buf = NumFieldFmt<WidthT, PaddingT>::of(aBuf.buf, aBuf.bufEnd, aField.value);
    return aBuf;
}

template<typename T, typename F>
struct FmtIf {
    bool predicate;
    T trueValue;
    F falseValue;

    FmtIf(const bool aPredicate,
          const T aTrueValue,
          const F aFalseValue)
        : predicate(aPredicate),
          trueValue(aTrueValue),
          falseValue(aFalseValue) {}
};

template<typename T, typename F>
inline FmtIf<T, F> fmtIf(const bool aPredicate,
                   const T aTrueValue,
                   const F aFalseValue) {
    return FmtIf<T, F>(aPredicate, aTrueValue, aFalseValue);
}

template<typename T, typename F> 
FmtBuf& operator<<(FmtBuf &aBuf, const FmtIf<T, F> &aFmtIf) {
    if (aFmtIf.predicate) {
        return aBuf << aFmtIf.trueValue;
    } else {
        return aBuf << aFmtIf.falseValue;
    }
}

struct Rewind {};

inline FmtBuf& operator<<(FmtBuf &aBuf, const Rewind&) {
    if (nebula_likely(aBuf.good()))
        aBuf.buf = aBuf.bufBegin;
    return aBuf;
}

struct ForceRewind {};

inline FmtBuf& operator<<(FmtBuf &aBuf, const ForceRewind&) {
    aBuf.buf = aBuf.bufBegin;
    return aBuf;
}

struct FmtBufSize {
    FmtBuf::size_type *sizePtr;
    explicit FmtBufSize(FmtBuf::size_type &aSize)
        : sizePtr(&aSize) {}
    explicit FmtBufSize(FmtBuf::size_type *aSizePtr)
        : sizePtr(aSizePtr) {}
};

inline FmtBuf& operator>>(FmtBuf &aBuf, FmtBufSize aSize) {
    *aSize.sizePtr = aBuf.size();
    return aBuf;
}

struct FmtBufPos {
    char **bufPtr;
    explicit FmtBufPos(char ** const aBufPtr)
        : bufPtr(aBufPtr) {}
};

inline FmtBuf& operator>>(FmtBuf &aBuf, FmtBufPos aPos) {
    *aPos.bufPtr = aBuf.buf;
    return aBuf;
}

template<typename A>
struct FmtBufClone {
    FmtBuf *fmtBufPtr;
    A action;
    explicit FmtBufClone(FmtBuf * const aFmtBufPtr,
                         const A anAction)
        : fmtBufPtr(aFmtBufPtr),
          action(anAction) {}
};

template<typename A>
FmtBufClone<A> fmtBufClone(FmtBuf * const aFmtBufPtr, const A anAction) {
    return FmtBufClone<A>(aFmtBufPtr, anAction);
}

template<typename A>
inline FmtBuf& operator>>(FmtBuf &aBuf, FmtBufClone<A> aClone) {
    *aClone.fmtBufPtr = aBuf;
    return aBuf >> aClone.action;
}

template<typename A>
inline FmtBuf& operator<<(FmtBuf &aBuf, FmtBufClone<A> aClone) {
    *aClone.fmtBufPtr = aBuf;
    return aBuf << aClone.action;
}

}}

#endif /* NEBULA_IO_FMTBUF_HPP */
