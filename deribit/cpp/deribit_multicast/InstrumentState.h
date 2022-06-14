/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _DERIBIT_MULTICAST_INSTRUMENTSTATE_H_
#define _DERIBIT_MULTICAST_INSTRUMENTSTATE_H_

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

class InstrumentState
{
public:
    enum Value
    {
        created = static_cast<std::uint8_t>(0),
        open = static_cast<std::uint8_t>(1),
        closed = static_cast<std::uint8_t>(2),
        settled = static_cast<std::uint8_t>(3),
        NULL_VALUE = static_cast<std::uint8_t>(255)
    };

    static InstrumentState::Value get(const std::uint8_t value)
    {
        switch (value)
        {
            case static_cast<std::uint8_t>(0): return created;
            case static_cast<std::uint8_t>(1): return open;
            case static_cast<std::uint8_t>(2): return closed;
            case static_cast<std::uint8_t>(3): return settled;
            case static_cast<std::uint8_t>(255): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum InstrumentState [E103]");
    }

    static const char *c_str(const InstrumentState::Value value)
    {
        switch (value)
        {
            case created: return "created";
            case open: return "open";
            case closed: return "closed";
            case settled: return "settled";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum InstrumentState [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, InstrumentState::Value m)
    {
        return os << InstrumentState::c_str(m);
    }
};

}

#endif
