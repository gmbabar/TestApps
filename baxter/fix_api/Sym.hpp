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

#ifndef _SYM_HPP
#define _SYM_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <cstddef>
#include <cstring>
#include <string>
#include <ostream>
#include <limits>


/*! \headerfile <Sym.hpp>
  \brief A symbol representation.
  \author Sebastian Hauer
  \ingroup tr
*/
struct Sym {
    static const std::size_t MaxSymbolSize = 63;
    static_assert(MaxSymbolSize <= std::numeric_limits<uint8_t>::max());

    /*! Default ctor of an symbol.
      Initializes to the empty string.
    */
    Sym() { 
        m_sym[0] = '\0'; 
        m_length = 1; 
    }

    /*! Ctor of an symbol.
      \param aSym  A zero terminated symbol string of less than MaxSymbolSize chars.
    */
    explicit Sym(const char * const aSym) {
        for(m_length=0; m_length < MaxSymbolSize - 1; ++m_length) {
            const char c = aSym[m_length];
            if(c != '\0')
                m_sym[m_length] = c;
            else
                break;
        }
        m_sym[m_length++] = '\0';
    }

    /*! Ctor of an symbol.
      \param aSym  A char pointer to an symbol string of less than MaxSymbolSize chars.
      \param aSymEnd  A pointer to the end of the symbol string.
    */
    Sym(const char * const aSym,
        const char * const aSymEnd) {
        m_length = std::min<std::size_t>(aSymEnd - aSym, MaxSymbolSize - 1);
        memcpy(m_sym, aSym, m_length);
        m_sym[m_length++] = '\0';
    }

    /*! Ctor of an symbol.
      \param Sym  Copy sym
    */
    Sym(const Sym &aSym) {
        m_length = aSym.m_length;
        memcpy(m_sym, aSym.m_sym, m_length);
    }

    inline const char* c_str() const {
        return m_sym;
    }
    inline std::string asStr() const {
        return this->c_str();
    }
    inline uint8_t length() const {
        return m_length;        
    }
    bool empty() const {
        return m_sym[0] == '\0';
    }

private:
    uint8_t m_length;
    char m_sym[MaxSymbolSize];
};

inline bool operator==(const Sym &lhs, const Sym &rhs) {
    return std::strncmp(lhs.c_str(), rhs.c_str(), Sym::MaxSymbolSize) == 0;
}

inline bool operator!=(const Sym &lhs, const Sym &rhs) {
    return std::strncmp(lhs.c_str(), rhs.c_str(), Sym::MaxSymbolSize) != 0;
}

inline bool operator>(const Sym &lhs, const Sym &rhs) {
    // A value greater than zero indicates that the first character that does not match has a greater value in lhs than in rhs
    return std::strncmp(lhs.c_str(), rhs.c_str(), Sym::MaxSymbolSize) > 0;
}

inline bool operator<(const Sym &lhs, const Sym &rhs) {
    // A value less than zero indicates that the first character that does not match has a lesser value in lhs than in rhs
    return std::strncmp(lhs.c_str(), rhs.c_str(), Sym::MaxSymbolSize) < 0;
}

namespace sym {

inline std::size_t hashValSym(std::size_t hashVal, const char* aSym) {
    for(unsigned i=0; i < Sym::MaxSymbolSize && *aSym != '\0'; ++i)
        boost::hash_combine(hashVal, *aSym++);
    return hashVal;
}

} // end namespace sym

/*! \headerfile <Sym.hpp>
  \brief Calculates the hash value of a symbol.
  This is an ADL overloaded function called by boost's unordered_map implementation.
  \author Sebastian Hauer
*/
inline std::size_t hash_value(const Sym &aSym) {
    return sym::hashValSym(0, aSym.c_str());
}

/*! \headerfile <Sym.hpp>
  \brief Calculates the hash value of an symbol while looping over the characters.
  \author Sebastian Hauer
*/
inline std::size_t hash_value_loop(const Sym &aSym) {
    std::size_t seed = 0;
    for (const char *chPtr = aSym.c_str(); *chPtr != '\0'; ++chPtr) {
        boost::hash_combine(seed, *chPtr);
    }
    return seed;
}

/*! \headerfile <Sym.hpp>
  \brief Calculates the hash value of an symbol.
  This is a type which is callable as an instance, provided here for alternative hash map
  implementations which permit the hashing algorithm to be specified as a callable type.
  \author Sebastian Hauer
*/
struct SymHash : public std::unary_function<Sym, std::size_t> {
    inline std::size_t operator()(const Sym &aSym) const {
        return hash_value(aSym);
    }
};

/*! \headerfile <Sym.hpp>
  \brief A symbol output operator.
  \author Sebastian Hauer
*/
 std::ostream& operator<<(std::ostream &o, const Sym &aSym);

/*! \headerfile <Sym.hpp>
  \brief Logs a symbol.
  E.g.:
  \code
  Sym sym("ESH0");
  log.dbg() << sym << log.endl();
  \endcode 

  \author Sebastian Hauer
  \ingroup tr
*/
template<template <typename> class LogStmt, typename BufT>
inline LogStmt<BufT> operator<<(LogStmt<BufT> aStmt, const Sym &aSym) {
    return aStmt << aSym.c_str();
}


#endif /* _SYM_HPP */
