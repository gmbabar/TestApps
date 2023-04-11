#ifndef _IO_NUMPADFMT_HPP
#define _IO_NUMPADFMT_HPP

#include "NumTraits.hpp"
#include "stdint.h"
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/concept_check.hpp>
#include <cstring>
#include <cassert>


namespace fmt_pad {

template<typename T, unsigned int Width, char Padding, bool IsSigned>
struct FmtPadNum {};

template<typename T, unsigned int Width, char Padding>
struct FmtPadNum<T, Width, Padding, false> {
private:
    BOOST_CONCEPT_ASSERT((boost::Integer<T>));
    static const char Zero = '0';
    static const unsigned int BufSz = (static_cast<unsigned int>(UIntDecMaxLen<T>::value) > Width 
                                       ? static_cast<unsigned int>(UIntDecMaxLen<T>::value)
                                       : Width);
public:
    static inline char* fmt(char * const aBuf, const char * const aBufEnd, const T aVal) {
        if (aBuf >= aBufEnd)
            return aBuf;
        T x = aVal;
        char buf[BufSz];
        unsigned int i = BufSz;
        do {
            buf[--i] = Zero + (x % 10);
            x /= 10;
            if (x == 0) break;
        } while (i > 0);

        const unsigned int n = BufSz - Width;
        if (i > n) {
            do {
                buf[--i] = Padding;
            } while (i > n);
        }
        assert(i < BufSz); 
        const std::size_t sz = BufSz - i;
        assert(sz <= BufSz); 
        if (aBuf + sz > aBufEnd) return aBuf + sz;
        std::memcpy(aBuf, buf + i, sz);
        return aBuf + sz;
    }
};

template<typename T, unsigned int Width, char Padding>
struct FmtPadNum<T, Width, Padding, true> {
private:
    BOOST_CONCEPT_ASSERT((boost::Integer<T>));
    typedef typename boost::make_unsigned<T>::type  unsigned_type;
    static const char Zero = '0';
    static const char Minus = '-';
    static const unsigned int BufSz = (static_cast<unsigned int>(UIntDecMaxLen<T>::value + 1) > Width 
                                       ? static_cast<unsigned int>(UIntDecMaxLen<T>::value + 1)
                                       : Width);
public:
    static inline char* fmt(char * const aBuf, const char * const aBufEnd, const T aVal) {
        if (aBuf >= aBufEnd)
            return aBuf;
        const T sign = aVal >= 0 ? 1 : -1;
        unsigned_type x = sign * aVal;
        char buf[BufSz];
        unsigned int i = BufSz;
        const unsigned int last = ((sign < 0) ? 1 : 0);
        do {
            buf[--i] = Zero + (x % 10);
            x /= 10;
            if (x == 0) break;
        } while (i > last);
        if (sign < 0) {
            buf[--i] = Minus;
        }
        const unsigned int n = (BufSz - Width);
        if (i > n) {
            do {
                buf[--i] = Padding;
            } while (i > n);
        }
        assert(i < BufSz); 
        const std::size_t sz = BufSz - i;
        assert(sz <= BufSz); 
        if (aBuf + sz > aBufEnd) return aBuf + sz;
        std::memcpy(aBuf, buf + i, sz);
        return aBuf + sz;
    }
};

} // end namespace fmt_pad


/*! \headerfile <NumPadFmt.hpp>
  \brief Formats a number of type T into a padded string representation.
  Should the string representation be shorter than the specified Width
  template parameter then it will left pad the string to the desired width.
  The unpadded string representation is written out should it be as wide or
  wider than the desired padding width.

  \note
  This function does not allocate its own heap space and it does
  not throw any exceptions.
  
  \param Width    The desired minimum width.
  \param Padding  The padding character, defaults to '0'.
  \author Sebastian Hauer
*/
template<unsigned int Width, char Padding = '0'>
struct NumPadFmt {
    /*! \brief The static inline method which does the padded formatting.
  
      \note
      This function will first check if aBuf is valid and not pointing to or
      past aBufEnd.  Should the write buffer pointer not be valid anymore
      it will simply return the invalid aBuf pointer.

      \param aBuf     The character buffer this function ought to write into.
      \param aBufEnd  The end of the character buffer which is always one
                      past the last buffer element (e.g. aBufEnd - 1).
      \param aVal     The integer number to format.
      \return  If successful returns the pointer to the char following the 
      last written byte.  Otherwise it will return a pointer past
      the last valid buffer character.
    */
    template<typename T>
    static inline char* of(char * const aBuf, const char * const aBufEnd, const T aVal) {
        return fmt_pad::FmtPadNum<T, Width, Padding, boost::is_signed<T>::value>::fmt(aBuf, aBufEnd, aVal);
    }
};


#endif /* _IO_NUMPADFMT_HPP */
