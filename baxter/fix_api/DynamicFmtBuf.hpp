#ifndef _IO_DYNAMIC_FMTBUF_HPP
#define _IO_DYNAMIC_FMTBUF_HPP

#include "FmtBuf.hpp"
#include "common.hpp"
#include "bufrange.hpp"
#include <string>
#include <cstddef>
#include <cstring>
#include <ostream>


template<typename Alloc = std::allocator<char> >
struct DynamicFmtBuf final {
    typedef std::size_t size_type;

    char* bufBegin;
    char* buf;
    char* bufEnd;

    Alloc allocator;
    bool failed_;


    DynamicFmtBuf()
        : bufBegin(nullptr),
        buf(nullptr),
        bufEnd(nullptr),
        allocator(), 
        failed_(false) {}

    DynamicFmtBuf(size_t size)
        :  DynamicFmtBuf() {

        bufBegin = allocator.allocate(size);
        buf = bufBegin;
        bufEnd = bufBegin + size;
    }
    
    DynamicFmtBuf(const DynamicFmtBuf&) = delete;
    DynamicFmtBuf(DynamicFmtBuf&&) = delete;
    DynamicFmtBuf& operator=(const DynamicFmtBuf&) = delete;
    DynamicFmtBuf& operator=(DynamicFmtBuf&&) = delete;


    ~DynamicFmtBuf() {
        allocator.deallocate(bufBegin, bufEnd - bufBegin);
    }

    inline size_type size() const {
        return buf >= bufBegin ? buf - bufBegin : 0;
    }

    inline DynamicFmtBuf& fail() {
        failed_ = true;
        return *this;
    }

    inline bool failed() const {
        return failed_;
    }

    inline constexpr bool done() const {
        return false;
    }

    inline bool good() const {
        return !failed_;
    }

    inline void ensureSpace(const size_t aSize) {
        if (bufBegin == nullptr) {
            bufBegin = allocator.allocate(aSize);
            buf = bufBegin;
            bufEnd = bufBegin + aSize;
            return;
        }
        if (buf + aSize > bufEnd) {
            const auto sz = size();
            const auto cap = static_cast<size_type>(bufEnd - bufBegin);
            auto newCap = cap * 2;
            while (newCap - sz < aSize
                    && newCap < allocator.max_size()) {
                newCap *= 2;
            }
            if (newCap >= allocator.max_size()) {
                failed_ = true;
                return;
            }
            auto p = allocator.allocate(newCap);
            memcpy(p, bufBegin, sz * sizeof(char));
            allocator.deallocate(bufBegin, cap);
            bufBegin = p;
            buf = bufBegin + sz;
            bufEnd = bufBegin + newCap;
        }
    }

};


template<typename A>
inline std::ostream& operator<<(std::ostream &o, const DynamicFmtBuf<A> &aBuf) {
    return o.write(aBuf.bufBegin, aBuf.size());
}

template<typename A>
inline bool operator==(const DynamicFmtBuf<A> &lhs, const DynamicFmtBuf<A> &rhs) {
    return lhs.good() && rhs.good() && lhs.size() == rhs.size()
        && std::strncmp(lhs.bufBegin, rhs.bufBegin, lhs.size()) == 0;
}

template<typename A>
inline bool operator!=(const DynamicFmtBuf<A> &lhs, const DynamicFmtBuf<A> &rhs) {
    return ! operator==(lhs, rhs);
}

template<typename A>
inline bool operator==(const DynamicFmtBuf<A> &lhs, const char * const aCStr) {
    return lhs.good() && lhs.size() == strlen(aCStr) 
        && std::strncmp(lhs.bufBegin, aCStr, lhs.size()) == 0;
}

template<typename A>
inline bool operator!=(const DynamicFmtBuf<A> &lhs, const char * const aCStr) {
    return ! operator==(lhs, aCStr);
}

template<typename A>
inline bool operator==(const char * const aCStr, const DynamicFmtBuf<A> &rhs) {
    return operator==(rhs, aCStr);
}

template<typename A>
inline bool operator!=(const char * const aCStr, const DynamicFmtBuf<A> &rhs) {
    return ! operator==(rhs, aCStr);
}


template<unsigned int N, typename T>
inline DynamicFmtBuf<T>& 
operator<<(DynamicFmtBuf<T> &aBuf, const Skip<N>&) {
    aBuf.ensureSpace(Skip<N>::Count);
    aBuf.buf += Skip<N>::Count;
    return aBuf;
}

template<typename T, typename BR>
typename std::enable_if<isOneOf<BR, ConstBufRange, 
                                    BufRange>::value,
                                    DynamicFmtBuf<T>&>::type
operator<<(DynamicFmtBuf<T> &aBuf, const BR &aRange) {
    const auto sz = aRange.size();
    aBuf.ensureSpace(sz);
    std::memcpy(aBuf.buf, aRange.buf, sz);    
    aBuf.buf += sz;
    return aBuf;
}

template<typename T>
inline DynamicFmtBuf<T>& 
operator<<(DynamicFmtBuf<T> &aBuf, const char * const aCStr) {
    const std::size_t sz = std::strlen(aCStr);
    aBuf.ensureSpace(sz);
    std::memcpy(aBuf.buf, aCStr, sz);
    aBuf.buf += sz;
    return aBuf;
}
template<typename T>
inline DynamicFmtBuf<T>& 
operator<<(DynamicFmtBuf<T> &aBuf, char * const aCStr) {
    const std::size_t sz = std::strlen(aCStr);
    aBuf.ensureSpace(sz);
    std::memcpy(aBuf.buf, aCStr, sz);
    aBuf.buf += sz;
    return aBuf;
}

template<typename T>
inline DynamicFmtBuf<T>& 
operator<<(DynamicFmtBuf<T> &aBuf, const std::string& aStr) {
    const std::size_t sz = aStr.length();
    aBuf.ensureSpace(sz);
    std::memcpy(aBuf.buf, aStr.c_str(), sz);
    aBuf.buf += sz;
    return aBuf;
}

template<unsigned int N, typename T>
inline DynamicFmtBuf<T>&
operator<<(DynamicFmtBuf<T> &aBuf, const char aChArr[N]) {
    if (N > 0) {
        const unsigned int sz = N - 1;
        aBuf.ensureSpace(sz);
        std::memcpy(aBuf.buf, aChArr, sz);
        aBuf.buf += sz;
    }
    return aBuf;
}

template<typename T, typename C>
inline
typename std::enable_if<isChar<C>::value, 
                        DynamicFmtBuf<T>&>::type
operator<<(DynamicFmtBuf<T> &aBuf, C aCh) {
    aBuf.ensureSpace(sizeof(C));
    *(aBuf.buf++) = aCh;
    return aBuf;
}

template<typename A, typename T>
inline
typename std::enable_if<std::is_integral<T>::value 
                        && !isChar<T>::value,
                        DynamicFmtBuf<A>&>::type
operator<<(DynamicFmtBuf<A> &aBuf, T aVal) {
    aBuf.ensureSpace(tml::IntegerMaxLen<T>::value);
    aBuf.buf = NumFmt::of(aBuf.buf, aBuf.bufEnd, aVal);
    return aBuf;
}

template<typename A, unsigned int Width, char Padding, typename T>
inline 
DynamicFmtBuf<A>&
operator<<(DynamicFmtBuf<A> &aBuf, PadNum<Width, Padding, T> aPadNum) {
    aBuf.ensureSpace(Width);
    aBuf.buf = NumPadFmt<Width, Padding>
                ::of(aBuf.buf, aBuf.bufEnd, aPadNum.val);
    return aBuf;
}

template<typename A, typename T>
inline
DynamicFmtBuf<A>& 
operator<<(DynamicFmtBuf<A> &aBuf, VarPrecision<T> aPrecVal) {
    // 22 = 21 (max unsigned integer digit number) + 1 dot
    aBuf.ensureSpace(22 + aPrecVal.prec);
    aBuf.buf = NumFmt::of(aBuf.buf, 
                            aBuf.bufEnd, 
                            aPrecVal.value, 
                            aPrecVal.prec);
    return aBuf;
}

template<typename A, typename T, unsigned int N> 
inline
DynamicFmtBuf<A>& 
operator<<(DynamicFmtBuf<A> &aBuf, Precision<T, N> aPrecVal) {
    aBuf.ensureSpace(22 + N);
    aBuf.buf = NumFmt::of(aBuf.buf, 
                            aBuf.bufEnd, 
                            aPrecVal.value, Precision<T, N>::Prec);
    return aBuf;
}

template<typename A, typename T, unsigned int WidthT, char PaddingT> 
inline
DynamicFmtBuf<A>&
operator<<(DynamicFmtBuf<A> &aBuf, Pad<T, WidthT, PaddingT> aPad) {
    aBuf.ensureSpace(WidthT);
    aBuf.buf = NumPadFmt<WidthT, PaddingT>::of(aBuf.buf, 
                                                aBuf.bufEnd, 
                                                aPad.value);
    return aBuf;
}

template<typename A, typename T, unsigned int WidthT, char PaddingT> 
inline
DynamicFmtBuf<A>& 
operator<<(DynamicFmtBuf<A> &aBuf, Field<T, WidthT, PaddingT> aField) {
    aBuf.ensureSpace(WidthT);
    aBuf.buf = NumFieldFmt<WidthT, PaddingT>::of(aBuf.buf, 
                                                    aBuf.bufEnd, 
                                                    aField.value);
    return aBuf;
}

template<typename A, typename T, typename F>
inline
DynamicFmtBuf<A>& 
operator<<(DynamicFmtBuf<A> &aBuf, const FmtIf<T, F> &aFmtIf) {
    if (aFmtIf.predicate) {
        return aBuf << aFmtIf.trueValue;
    } else {
        return aBuf << aFmtIf.falseValue;
    }
}

template<typename A>
inline 
DynamicFmtBuf<A>& 
operator<<(DynamicFmtBuf<A> &aBuf, const Rewind&) {
    if (aBuf.good())
        aBuf.buf = aBuf.bufBegin;
    return aBuf;
}

template<typename A>
inline 
DynamicFmtBuf<A>& 
operator<<(DynamicFmtBuf<A> &aBuf, const ForceRewind&) {
    aBuf.buf = aBuf.bufBegin;
    return aBuf;
}


#endif /* _IO_DYNAMIC_FMTBUF_HPP */
