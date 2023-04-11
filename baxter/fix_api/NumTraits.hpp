#ifndef _TML_NUMTRAITS_HPP
#define _TML_NUMTRAITS_HPP

#include <cstddef>

template<std::size_t N>
struct UIntDecMaxLenVal {
    struct ERROR__Invalid_sizeof_number {};
    struct value : ERROR__Invalid_sizeof_number {};
};
template<> struct UIntDecMaxLenVal<1> { enum { value = 3 }; };
template<> struct UIntDecMaxLenVal<2> { enum { value = 5 }; };
template<> struct UIntDecMaxLenVal<4> { enum { value = 10 }; };
template<> struct UIntDecMaxLenVal<8> { enum { value = 20 }; };

template<typename T>
struct UIntDecMaxLen { enum { value = UIntDecMaxLenVal<sizeof(T)>::value }; };

template<std::size_t N>
struct SIntDecMaxLenVal {
    struct ERROR__Invalid_sizeof_number {};
    struct value : ERROR__Invalid_sizeof_number {};
};
template<> struct SIntDecMaxLenVal<1> { enum { value = 4 }; };
template<> struct SIntDecMaxLenVal<2> { enum { value = 6 }; };
template<> struct SIntDecMaxLenVal<4> { enum { value = 11 }; };
template<> struct SIntDecMaxLenVal<8> { enum { value = 21 }; };

template<typename T>
struct SIntDecMaxLen { enum { value = SIntDecMaxLenVal<sizeof(T)>::value }; };

namespace {

#ifdef GEN_COND_INT
    #error "GEN_COND_INT redefined!"
#else 

#define GEN_COND_INT(IntT)\
template<bool B, IntT I1, IntT I2>\
struct conditional##IntT { enum {value = I1}; };\
template<IntT I1, IntT I2>\
struct conditional##IntT<false, I1, I2> { enum {value = I2};};


GEN_COND_INT(int8_t)
GEN_COND_INT(int16_t)
GEN_COND_INT(int32_t)
GEN_COND_INT(int64_t)

GEN_COND_INT(uint8_t)
GEN_COND_INT(uint16_t)
GEN_COND_INT(uint32_t)
GEN_COND_INT(uint64_t)

#undef GEN_COND_INT

#endif
}

template<typename T>
struct IntegerMaxLen { 
    enum {
        value = conditionalint32_t<std::is_signed<T>::value,
                            SIntDecMaxLen<T>::value,
                            UIntDecMaxLen<T>::value>::value
    };
};


template<typename T> struct FloatingTraits {
    struct ERROR__Invalid_floating_point_type {};
    static const ERROR__Invalid_floating_point_type MaxSignificant;
    static const ERROR__Invalid_floating_point_type MaxPrecision;
};

template<> struct FloatingTraits<float> {
    static const uint64_t MaxSignificant = 8388607ULL; // 2^23 - 1
    static const unsigned int MaxPrecision = 7;
};
template<> struct FloatingTraits<double> {
    static const uint64_t MaxSignificant = 4503599627370495ULL; // 2^52 - 1
    static const unsigned int MaxPrecision = 16;
};
template<> struct FloatingTraits<long double> {
    static const uint64_t MaxSignificant = 18446744073709551615ULL; // 2^64 - 1
    static const unsigned int MaxPrecision = 19;
};



#endif /* _TML_NUMTRAITS_HPP */
