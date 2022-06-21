/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _DERIBIT_MULTICAST_OPTIONTYPE_H_
#define _DERIBIT_MULTICAST_OPTIONTYPE_H_

#if !defined(__STDC_LIMIT_MACROS)
#  define __STDC_LIMIT_MACROS 1
#endif

#include <cstdint>
#include <iomanip>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <sstream>
#include <string>

#define SBE_NULLVALUE_INT8 (std::numeric_limits<std::int8_t>::min)()
#define SBE_NULLVALUE_INT16 (std::numeric_limits<std::int16_t>::min)()
#define SBE_NULLVALUE_INT32 (std::numeric_limits<std::int32_t>::min)()
#define SBE_NULLVALUE_INT64 (std::numeric_limits<std::int64_t>::min)()
#define SBE_NULLVALUE_UINT8 (std::numeric_limits<std::uint8_t>::max)()
#define SBE_NULLVALUE_UINT16 (std::numeric_limits<std::uint16_t>::max)()
#define SBE_NULLVALUE_UINT32 (std::numeric_limits<std::uint32_t>::max)()
#define SBE_NULLVALUE_UINT64 (std::numeric_limits<std::uint64_t>::max)()

namespace deribit_multicast {

class OptionType
{
public:
    enum Value
    {
        not_applicable = static_cast<std::uint8_t>(0),
        put = static_cast<std::uint8_t>(1),
        call = static_cast<std::uint8_t>(2),
        NULL_VALUE = static_cast<std::uint8_t>(255)
    };

    static OptionType::Value get(const std::uint8_t value)
    {
        switch (value)
        {
            case static_cast<std::uint8_t>(0): return not_applicable;
            case static_cast<std::uint8_t>(1): return put;
            case static_cast<std::uint8_t>(2): return call;
            case static_cast<std::uint8_t>(255): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum OptionType [E103]");
    }

    static const char *c_str(const OptionType::Value value)
    {
        switch (value)
        {
            case not_applicable: return "not_applicable";
            case put: return "put";
            case call: return "call";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum OptionType [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, OptionType::Value m)
    {
        return os << OptionType::c_str(m);
    }
};

}

#endif
