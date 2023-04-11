#ifndef _DATA_STDINT_H
#define _DATA_STDINT_H

#if defined(sun) || defined(__sun__)
#  define _HAS_INTTYPES_H
#elif !defined(_MSC_VER)
#  define _HAS_STDINT_H
#endif

#if defined(_HAS_INTTYPES_H)
#  include <inttypes.h>
#elif defined(_HAS_STDINT_H)
#  include <stdint.h>
#elif defined(_MSC_VER)
#  include <limits.h>
   typedef signed char     int8_t;
   typedef signed char     int_least8_t;
   typedef signed char     int_fast8_t;
   typedef unsigned char   uint8_t;
   typedef unsigned char   uint_least8_t;
   typedef unsigned char   uint_fast8_t;

   typedef short           int16_t;
   typedef short           int_least16_t;
   typedef short           int_fast16_t;
   typedef unsigned short  uint16_t;
   typedef unsigned short  uint_least16_t;
   typedef unsigned short  uint_fast16_t;

#  if ULONG_MAX == 0xffffffff
     typedef long            int32_t;
     typedef long            int_least32_t;
     typedef long            int_fast32_t;
     typedef unsigned long   uint32_t;
     typedef unsigned long   uint_least32_t;
     typedef unsigned long   uint_fast32_t;
#  elif UINT_MAX == 0xffffffff
     typedef int             int32_t;
     typedef int             int_least32_t;
     typedef int             int_fast32_t;
     typedef unsigned int    uint32_t;
     typedef unsigned int    uint_least32_t;
     typedef unsigned int    uint_fast32_t;
#  else
#    error defaults not correct; you must hand modify /data/stdint.h
#  endif

   typedef __int64             intmax_t;
   typedef unsigned __int64    uintmax_t;
   typedef __int64             int64_t;
   typedef __int64             int_least64_t;
   typedef __int64             int_fast64_t;
   typedef unsigned __int64    uint64_t;
   typedef unsigned __int64    uint_least64_t;
   typedef unsigned __int64    uint_fast64_t;
#else
#  error Unknown platform unable to determine how to define fixed size integer types
#endif

#endif /* _DATA_STDINT_H */
