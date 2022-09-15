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

#ifndef NEBULA_COMMON_H
#define NEBULA_COMMON_H

// #include "debug.hpp"`

/*!
  \defgroup common  Common
  \defgroup utils   Utilities
  \defgroup tr      Trading
  \defgroup cfg     Config
  \defgroup log     Logging
*/


/*! \def NEBULA_DECL
  \headerfile <nebula/common.hpp>
  \brief If dynamic linking is enabled via NEBULA_DYN_LINK
  it determines which symbol gets exported.

  \author Sebastian Hauer
  \ingroup common 
*/

#ifdef _MSC_VER  
/* Microsoft Visual C++ */   
#  ifdef NEBULA_DYN_LINK
   /* export if we are linking our own source, otherwise import: */
#    define NEBULA_DECL __declspec(dllexport)
#  else
#    define NEBULA_DECL __declspec(dllimport)
#  endif /* NEBULA_DYN_LINK */
#else
#    define NEBULA_DECL 
#endif /* _MSC_VER   */

#define NEBULA_STRINGIFY(X) #X
#define NEBULA_EVAL_STRINGIFY(X) NEBULA_STRINGIFY(X)

#ifdef __cplusplus
#    define NEBULA_EXTERN_C extern "C"
#    define NEBULA_BEGIN_EXTERN_C extern "C" {
#    define NEBULA_END_EXTERN_C }
#else
#    define NEBULA_EXTERN_C 
#    define NEBULA_BEGIN_EXTERN_C
#    define NEBULA_END_EXTERN_C
#endif

#if defined(__GNUC__)

#define NEBULA_PRAGMA(x) _Pragma (#x)
#define NEBULA_TODO(x) NEBULA_PRAGMA(message ("TODO - " x))
#define NEBULA_WARN(x) NEBULA_PRAGMA(message ("WARN - " x))
#define NEBULA_NOTICE(x) NEBULA_PRAGMA(message ("NOTICE - " x))
#define NEBULA_WARNING_ON(x) NEBULA_PRAGMA(GCC diagnostic warning #x)
#define NEBULA_WARNING_OFF(x) NEBULA_PRAGMA (GCC diagnostic ignored #x)
#define NEBULA_HOT __attribute__((hot))
#define NEBULA_NOINLINE __attribute__((noinline))
#define NEBULA_ALWAYSINLINE __attribute__((always_inline))
#define NEBULA_PACKED_ATTRIB __attribute__((packed))
#define NEBULA_NOEXCEPT noexcept
#define NEBULA_PREFETCH(addy, locality) __builtin_prefetch(reinterpret_cast<void*>(addy), locality);

#else

#define NEBULA_TODO(x) 
#define NEBULA_WARN(x) 
#define NEBULA_NOTICE(x)
#define NEBULA_WARNING_ON(x)
#define NEBULA_WARNING_OFF(x)
#define NEBULA_HOT
#define NEBULA_NOINLINE
#define NEBULA_ALWAYSINLINE 
#define NEBULA_PACKED_ATTRIB 
#define NEBULA_NOEXCEPT
#define NEBULA_PREFETCH(addy, locality) 

#endif

/* Nebula Version */
#define VAL(str) #str
#define TOSTRING(str) VAL(str)

#ifndef NEBULA_VERSION
    #define NEBULA_VERSION Unknown
#endif

namespace nebula { namespace common {

inline const char * nebulaVersion() {
    return TOSTRING(NEBULA_VERSION);
}

}}

#undef VAL
#undef TOSTRING


#endif /* NEBULA_COMMON_H */
