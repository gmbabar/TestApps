/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _DERIBIT_MULTICAST_PERIOD_H_
#define _DERIBIT_MULTICAST_PERIOD_H_

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

class Period
{
public:
    enum Value
    {
        perpetual = static_cast<std::uint8_t>(0),
        minute = static_cast<std::uint8_t>(1),
        hour = static_cast<std::uint8_t>(2),
        day = static_cast<std::uint8_t>(3),
        week = static_cast<std::uint8_t>(4),
        month = static_cast<std::uint8_t>(5),
        year = static_cast<std::uint8_t>(6),
        NULL_VALUE = static_cast<std::uint8_t>(255)
    };

    static Period::Value get(const std::uint8_t value)
    {
        switch (value)
        {
            case static_cast<std::uint8_t>(0): return perpetual;
            case static_cast<std::uint8_t>(1): return minute;
            case static_cast<std::uint8_t>(2): return hour;
            case static_cast<std::uint8_t>(3): return day;
            case static_cast<std::uint8_t>(4): return week;
            case static_cast<std::uint8_t>(5): return month;
            case static_cast<std::uint8_t>(6): return year;
            case static_cast<std::uint8_t>(255): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum Period [E103]");
    }

    static const char *c_str(const Period::Value value)
    {
        switch (value)
        {
            case perpetual: return "perpetual";
            case minute: return "minute";
            case hour: return "hour";
            case day: return "day";
            case week: return "week";
            case month: return "month";
            case year: return "year";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum Period [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, Period::Value m)
    {
        return os << Period::c_str(m);
    }
};

}

#endif
