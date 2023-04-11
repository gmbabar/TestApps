#ifndef _COMMON_HPP_
#define _COMMON_HPP_
#include "StackAlloc.hpp"


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

template<unsigned int N>
struct Skip {
    enum { Count = N };
};

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


#endif