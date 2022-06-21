/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _DERIBIT_MULTICAST_INSTRUMENT_H_
#define _DERIBIT_MULTICAST_INSTRUMENT_H_

#if defined(SBE_HAVE_CMATH)
/* cmath needed for std::numeric_limits<double>::quiet_NaN() */
#  include <cmath>
#  define SBE_FLOAT_NAN std::numeric_limits<float>::quiet_NaN()
#  define SBE_DOUBLE_NAN std::numeric_limits<double>::quiet_NaN()
#else
/* math.h needed for NAN */
#  include <math.h>
#  define SBE_FLOAT_NAN NAN
#  define SBE_DOUBLE_NAN NAN
#endif

#if __cplusplus >= 201103L
#  define SBE_CONSTEXPR constexpr
#  define SBE_NOEXCEPT noexcept
#else
#  define SBE_CONSTEXPR
#  define SBE_NOEXCEPT
#endif

#if __cplusplus >= 201703L
#  include <string_view>
#  define SBE_NODISCARD [[nodiscard]]
#else
#  define SBE_NODISCARD
#endif

#if !defined(__STDC_LIMIT_MACROS)
#  define __STDC_LIMIT_MACROS 1
#endif

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

#if defined(WIN32) || defined(_WIN32)
#  define SBE_BIG_ENDIAN_ENCODE_16(v) _byteswap_ushort(v)
#  define SBE_BIG_ENDIAN_ENCODE_32(v) _byteswap_ulong(v)
#  define SBE_BIG_ENDIAN_ENCODE_64(v) _byteswap_uint64(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_16(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_32(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_64(v) (v)
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define SBE_BIG_ENDIAN_ENCODE_16(v) __builtin_bswap16(v)
#  define SBE_BIG_ENDIAN_ENCODE_32(v) __builtin_bswap32(v)
#  define SBE_BIG_ENDIAN_ENCODE_64(v) __builtin_bswap64(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_16(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_32(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_64(v) (v)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define SBE_LITTLE_ENDIAN_ENCODE_16(v) __builtin_bswap16(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_32(v) __builtin_bswap32(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_64(v) __builtin_bswap64(v)
#  define SBE_BIG_ENDIAN_ENCODE_16(v) (v)
#  define SBE_BIG_ENDIAN_ENCODE_32(v) (v)
#  define SBE_BIG_ENDIAN_ENCODE_64(v) (v)
#else
#  error "Byte Ordering of platform not determined. Set __BYTE_ORDER__ manually before including this file."
#endif

#if defined(SBE_NO_BOUNDS_CHECK)
#  define SBE_BOUNDS_CHECK_EXPECT(exp, c) (false)
#elif defined(_MSC_VER)
#  define SBE_BOUNDS_CHECK_EXPECT(exp, c) (exp)
#else
#  define SBE_BOUNDS_CHECK_EXPECT(exp, c) (__builtin_expect(exp, c))
#endif

#define SBE_NULLVALUE_INT8 (std::numeric_limits<std::int8_t>::min)()
#define SBE_NULLVALUE_INT16 (std::numeric_limits<std::int16_t>::min)()
#define SBE_NULLVALUE_INT32 (std::numeric_limits<std::int32_t>::min)()
#define SBE_NULLVALUE_INT64 (std::numeric_limits<std::int64_t>::min)()
#define SBE_NULLVALUE_UINT8 (std::numeric_limits<std::uint8_t>::max)()
#define SBE_NULLVALUE_UINT16 (std::numeric_limits<std::uint16_t>::max)()
#define SBE_NULLVALUE_UINT32 (std::numeric_limits<std::uint32_t>::max)()
#define SBE_NULLVALUE_UINT64 (std::numeric_limits<std::uint64_t>::max)()


#include "Liquidation.h"
#include "Period.h"
#include "MessageHeader.h"
#include "InstrumentState.h"
#include "InstrumentKind.h"
#include "VarString.h"
#include "GroupSizeEncoding.h"
#include "OptionType.h"
#include "YesNo.h"
#include "TickDirection.h"
#include "FutureType.h"
#include "BookSide.h"
#include "BookChange.h"
#include "Direction.h"

namespace deribit_multicast {

class Instrument
{
private:
    char *m_buffer = nullptr;
    std::uint64_t m_bufferLength = 0;
    std::uint64_t m_offset = 0;
    std::uint64_t m_position = 0;
    std::uint64_t m_actingBlockLength = 0;
    std::uint64_t m_actingVersion = 0;

    inline std::uint64_t *sbePositionPtr() SBE_NOEXCEPT
    {
        return &m_position;
    }

public:
    static const std::uint16_t SBE_BLOCK_LENGTH = static_cast<std::uint16_t>(152);
    static const std::uint16_t SBE_TEMPLATE_ID = static_cast<std::uint16_t>(1000);
    static const std::uint16_t SBE_SCHEMA_ID = static_cast<std::uint16_t>(1);
    static const std::uint16_t SBE_SCHEMA_VERSION = static_cast<std::uint16_t>(1);

    enum MetaAttribute
    {
        EPOCH, TIME_UNIT, SEMANTIC_TYPE, PRESENCE
    };

    union sbe_float_as_uint_u
    {
        float fp_value;
        std::uint32_t uint_value;
    };

    union sbe_double_as_uint_u
    {
        double fp_value;
        std::uint64_t uint_value;
    };

    using messageHeader = MessageHeader;

    Instrument() = default;

    Instrument(
        char *buffer,
        const std::uint64_t offset,
        const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion) :
        m_buffer(buffer),
        m_bufferLength(bufferLength),
        m_offset(offset),
        m_position(sbeCheckPosition(offset + actingBlockLength)),
        m_actingBlockLength(actingBlockLength),
        m_actingVersion(actingVersion)
    {
    }

    Instrument(char *buffer, const std::uint64_t bufferLength) :
        Instrument(buffer, 0, bufferLength, sbeBlockLength(), sbeSchemaVersion())
    {
    }

    Instrument(
        char *buffer,
        const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion) :
        Instrument(buffer, 0, bufferLength, actingBlockLength, actingVersion)
    {
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeBlockLength() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(152);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t sbeBlockAndHeaderLength() SBE_NOEXCEPT
    {
        return messageHeader::encodedLength() + sbeBlockLength();
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeTemplateId() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(1000);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeSchemaId() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(1);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeSchemaVersion() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(1);
    }

    SBE_NODISCARD static SBE_CONSTEXPR const char *sbeSemanticType() SBE_NOEXCEPT
    {
        return "";
    }

    SBE_NODISCARD std::uint64_t offset() const SBE_NOEXCEPT
    {
        return m_offset;
    }

    Instrument &wrapForEncode(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        return *this = Instrument(buffer, offset, bufferLength, sbeBlockLength(), sbeSchemaVersion());
    }

    Instrument &wrapAndApplyHeader(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        messageHeader hdr(buffer, offset, bufferLength, sbeSchemaVersion());

        hdr
            .blockLength(sbeBlockLength())
            .templateId(sbeTemplateId())
            .schemaId(sbeSchemaId())
            .version(sbeSchemaVersion());

        return *this = Instrument(
            buffer,
            offset + messageHeader::encodedLength(),
            bufferLength,
            sbeBlockLength(),
            sbeSchemaVersion());
    }

    Instrument &wrapForDecode(
        char *buffer,
        const std::uint64_t offset,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion,
        const std::uint64_t bufferLength)
    {
        return *this = Instrument(buffer, offset, bufferLength, actingBlockLength, actingVersion);
    }

    Instrument &sbeRewind()
    {
        return wrapForDecode(m_buffer, m_offset, m_actingBlockLength, m_actingVersion, m_bufferLength);
    }

    SBE_NODISCARD std::uint64_t sbePosition() const SBE_NOEXCEPT
    {
        return m_position;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    std::uint64_t sbeCheckPosition(const std::uint64_t position)
    {
        if (SBE_BOUNDS_CHECK_EXPECT((position > m_bufferLength), false))
        {
            throw std::runtime_error("buffer too short [E100]");
        }
        return position;
    }

    void sbePosition(const std::uint64_t position)
    {
        m_position = sbeCheckPosition(position);
    }

    SBE_NODISCARD std::uint64_t encodedLength() const SBE_NOEXCEPT
    {
        return sbePosition() - m_offset;
    }

    SBE_NODISCARD std::uint64_t decodeLength() const
    {
        Instrument skipper(m_buffer, m_offset, m_bufferLength, sbeBlockLength(), m_actingVersion);
        skipper.skip();
        return skipper.encodedLength();
    }

    SBE_NODISCARD const char *buffer() const SBE_NOEXCEPT
    {
        return m_buffer;
    }

    SBE_NODISCARD char *buffer() SBE_NOEXCEPT
    {
        return m_buffer;
    }

    SBE_NODISCARD std::uint64_t bufferLength() const SBE_NOEXCEPT
    {
        return m_bufferLength;
    }

    SBE_NODISCARD std::uint64_t actingVersion() const SBE_NOEXCEPT
    {
        return m_actingVersion;
    }

    SBE_NODISCARD static const char *headerMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t headerId() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t headerSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool headerInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= headerSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t headerEncodingOffset() SBE_NOEXCEPT
    {
        return 0;
    }

private:
    MessageHeader m_header;

public:
    SBE_NODISCARD MessageHeader &header()
    {
        m_header.wrap(m_buffer, m_offset + 0, m_actingVersion, m_bufferLength);
        return m_header;
    }

    SBE_NODISCARD static const char *instrumentIdMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t instrumentIdId() SBE_NOEXCEPT
    {
        return 2;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t instrumentIdSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool instrumentIdInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= instrumentIdSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t instrumentIdEncodingOffset() SBE_NOEXCEPT
    {
        return 12;
    }

    static SBE_CONSTEXPR std::uint32_t instrumentIdNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT32;
    }

    static SBE_CONSTEXPR std::uint32_t instrumentIdMinValue() SBE_NOEXCEPT
    {
        return UINT32_C(0x0);
    }

    static SBE_CONSTEXPR std::uint32_t instrumentIdMaxValue() SBE_NOEXCEPT
    {
        return UINT32_C(0xfffffffe);
    }

    static SBE_CONSTEXPR std::size_t instrumentIdEncodingLength() SBE_NOEXCEPT
    {
        return 4;
    }

    SBE_NODISCARD std::uint32_t instrumentId() const SBE_NOEXCEPT
    {
        std::uint32_t val;
        std::memcpy(&val, m_buffer + m_offset + 12, sizeof(std::uint32_t));
        return SBE_LITTLE_ENDIAN_ENCODE_32(val);
    }

    Instrument &instrumentId(const std::uint32_t value) SBE_NOEXCEPT
    {
        std::uint32_t val = SBE_LITTLE_ENDIAN_ENCODE_32(value);
        std::memcpy(m_buffer + m_offset + 12, &val, sizeof(std::uint32_t));
        return *this;
    }

    SBE_NODISCARD static const char *instrumentStateMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t instrumentStateId() SBE_NOEXCEPT
    {
        return 3;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t instrumentStateSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool instrumentStateInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= instrumentStateSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t instrumentStateEncodingOffset() SBE_NOEXCEPT
    {
        return 16;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t instrumentStateEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t instrumentStateRaw() const SBE_NOEXCEPT
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 16, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD InstrumentState::Value instrumentState() const
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 16, sizeof(std::uint8_t));
        return InstrumentState::get((val));
    }

    Instrument &instrumentState(const InstrumentState::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 16, &val, sizeof(std::uint8_t));
        return *this;
    }

    SBE_NODISCARD static const char *kindMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t kindId() SBE_NOEXCEPT
    {
        return 4;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t kindSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool kindInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= kindSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t kindEncodingOffset() SBE_NOEXCEPT
    {
        return 17;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t kindEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t kindRaw() const SBE_NOEXCEPT
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 17, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD InstrumentKind::Value kind() const
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 17, sizeof(std::uint8_t));
        return InstrumentKind::get((val));
    }

    Instrument &kind(const InstrumentKind::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 17, &val, sizeof(std::uint8_t));
        return *this;
    }

    SBE_NODISCARD static const char *futureTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t futureTypeId() SBE_NOEXCEPT
    {
        return 5;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t futureTypeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool futureTypeInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= futureTypeSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t futureTypeEncodingOffset() SBE_NOEXCEPT
    {
        return 18;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t futureTypeEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t futureTypeRaw() const SBE_NOEXCEPT
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 18, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD FutureType::Value futureType() const
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 18, sizeof(std::uint8_t));
        return FutureType::get((val));
    }

    Instrument &futureType(const FutureType::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 18, &val, sizeof(std::uint8_t));
        return *this;
    }

    SBE_NODISCARD static const char *optionTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t optionTypeId() SBE_NOEXCEPT
    {
        return 6;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t optionTypeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool optionTypeInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= optionTypeSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t optionTypeEncodingOffset() SBE_NOEXCEPT
    {
        return 19;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t optionTypeEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t optionTypeRaw() const SBE_NOEXCEPT
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 19, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD OptionType::Value optionType() const
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 19, sizeof(std::uint8_t));
        return OptionType::get((val));
    }

    Instrument &optionType(const OptionType::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 19, &val, sizeof(std::uint8_t));
        return *this;
    }

    SBE_NODISCARD static const char *rfqMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t rfqId() SBE_NOEXCEPT
    {
        return 7;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t rfqSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool rfqInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= rfqSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t rfqEncodingOffset() SBE_NOEXCEPT
    {
        return 20;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t rfqEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t rfqRaw() const SBE_NOEXCEPT
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 20, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD YesNo::Value rfq() const
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 20, sizeof(std::uint8_t));
        return YesNo::get((val));
    }

    Instrument &rfq(const YesNo::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 20, &val, sizeof(std::uint8_t));
        return *this;
    }

    SBE_NODISCARD static const char *settlementPeriodMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t settlementPeriodId() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t settlementPeriodSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool settlementPeriodInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= settlementPeriodSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t settlementPeriodEncodingOffset() SBE_NOEXCEPT
    {
        return 21;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t settlementPeriodEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t settlementPeriodRaw() const SBE_NOEXCEPT
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 21, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD Period::Value settlementPeriod() const
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 21, sizeof(std::uint8_t));
        return Period::get((val));
    }

    Instrument &settlementPeriod(const Period::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 21, &val, sizeof(std::uint8_t));
        return *this;
    }

    SBE_NODISCARD static const char *settlementPeriodCountMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t settlementPeriodCountId() SBE_NOEXCEPT
    {
        return 9;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t settlementPeriodCountSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool settlementPeriodCountInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= settlementPeriodCountSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t settlementPeriodCountEncodingOffset() SBE_NOEXCEPT
    {
        return 22;
    }

    static SBE_CONSTEXPR std::uint16_t settlementPeriodCountNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT16;
    }

    static SBE_CONSTEXPR std::uint16_t settlementPeriodCountMinValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(0);
    }

    static SBE_CONSTEXPR std::uint16_t settlementPeriodCountMaxValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(65534);
    }

    static SBE_CONSTEXPR std::size_t settlementPeriodCountEncodingLength() SBE_NOEXCEPT
    {
        return 2;
    }

    SBE_NODISCARD std::uint16_t settlementPeriodCount() const SBE_NOEXCEPT
    {
        std::uint16_t val;
        std::memcpy(&val, m_buffer + m_offset + 22, sizeof(std::uint16_t));
        return SBE_LITTLE_ENDIAN_ENCODE_16(val);
    }

    Instrument &settlementPeriodCount(const std::uint16_t value) SBE_NOEXCEPT
    {
        std::uint16_t val = SBE_LITTLE_ENDIAN_ENCODE_16(value);
        std::memcpy(m_buffer + m_offset + 22, &val, sizeof(std::uint16_t));
        return *this;
    }

    SBE_NODISCARD static const char *baseCurrencyMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t baseCurrencyId() SBE_NOEXCEPT
    {
        return 10;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t baseCurrencySinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool baseCurrencyInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= baseCurrencySinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t baseCurrencyEncodingOffset() SBE_NOEXCEPT
    {
        return 24;
    }

    static SBE_CONSTEXPR char baseCurrencyNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char baseCurrencyMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char baseCurrencyMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t baseCurrencyEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t baseCurrencyLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD const char *baseCurrency() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 24;
    }

    SBE_NODISCARD char *baseCurrency() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 24;
    }

    SBE_NODISCARD char baseCurrency(const std::uint64_t index) const
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for baseCurrency [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 24 + (index * 1), sizeof(char));
        return (val);
    }

    Instrument &baseCurrency(const std::uint64_t index, const char value)
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for baseCurrency [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 24 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getBaseCurrency(char *const dst, const std::uint64_t length) const
    {
        if (length > 8)
        {
            throw std::runtime_error("length too large for getBaseCurrency [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 24, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    Instrument &putBaseCurrency(const char *const src) SBE_NOEXCEPT
    {
        std::memcpy(m_buffer + m_offset + 24, src, sizeof(char) * 8);
        return *this;
    }

    SBE_NODISCARD std::string getBaseCurrencyAsString() const
    {
        const char *buffer = m_buffer + m_offset + 24;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getBaseCurrencyAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getBaseCurrencyAsString();

        for (const auto c : s)
        {
            switch (c)
            {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;

                default:
                    if ('\x00' <= c && c <= '\x1f')
                    {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    }
                    else
                    {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

    #if __cplusplus >= 201703L
    SBE_NODISCARD std::string_view getBaseCurrencyAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 24;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #if __cplusplus >= 201703L
    Instrument &putBaseCurrency(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putBaseCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 24, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 24 + start] = 0;
        }

        return *this;
    }
    #else
    Instrument &putBaseCurrency(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putBaseCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 24, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 24 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *quoteCurrencyMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t quoteCurrencyId() SBE_NOEXCEPT
    {
        return 11;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t quoteCurrencySinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool quoteCurrencyInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= quoteCurrencySinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t quoteCurrencyEncodingOffset() SBE_NOEXCEPT
    {
        return 32;
    }

    static SBE_CONSTEXPR char quoteCurrencyNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char quoteCurrencyMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char quoteCurrencyMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t quoteCurrencyEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t quoteCurrencyLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD const char *quoteCurrency() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 32;
    }

    SBE_NODISCARD char *quoteCurrency() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 32;
    }

    SBE_NODISCARD char quoteCurrency(const std::uint64_t index) const
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for quoteCurrency [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 32 + (index * 1), sizeof(char));
        return (val);
    }

    Instrument &quoteCurrency(const std::uint64_t index, const char value)
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for quoteCurrency [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 32 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getQuoteCurrency(char *const dst, const std::uint64_t length) const
    {
        if (length > 8)
        {
            throw std::runtime_error("length too large for getQuoteCurrency [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 32, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    Instrument &putQuoteCurrency(const char *const src) SBE_NOEXCEPT
    {
        std::memcpy(m_buffer + m_offset + 32, src, sizeof(char) * 8);
        return *this;
    }

    SBE_NODISCARD std::string getQuoteCurrencyAsString() const
    {
        const char *buffer = m_buffer + m_offset + 32;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getQuoteCurrencyAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getQuoteCurrencyAsString();

        for (const auto c : s)
        {
            switch (c)
            {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;

                default:
                    if ('\x00' <= c && c <= '\x1f')
                    {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    }
                    else
                    {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

    #if __cplusplus >= 201703L
    SBE_NODISCARD std::string_view getQuoteCurrencyAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 32;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #if __cplusplus >= 201703L
    Instrument &putQuoteCurrency(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putQuoteCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 32, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 32 + start] = 0;
        }

        return *this;
    }
    #else
    Instrument &putQuoteCurrency(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putQuoteCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 32, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 32 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *counterCurrencyMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t counterCurrencyId() SBE_NOEXCEPT
    {
        return 12;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t counterCurrencySinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool counterCurrencyInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= counterCurrencySinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t counterCurrencyEncodingOffset() SBE_NOEXCEPT
    {
        return 40;
    }

    static SBE_CONSTEXPR char counterCurrencyNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char counterCurrencyMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char counterCurrencyMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t counterCurrencyEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t counterCurrencyLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD const char *counterCurrency() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 40;
    }

    SBE_NODISCARD char *counterCurrency() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 40;
    }

    SBE_NODISCARD char counterCurrency(const std::uint64_t index) const
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for counterCurrency [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 40 + (index * 1), sizeof(char));
        return (val);
    }

    Instrument &counterCurrency(const std::uint64_t index, const char value)
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for counterCurrency [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 40 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getCounterCurrency(char *const dst, const std::uint64_t length) const
    {
        if (length > 8)
        {
            throw std::runtime_error("length too large for getCounterCurrency [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 40, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    Instrument &putCounterCurrency(const char *const src) SBE_NOEXCEPT
    {
        std::memcpy(m_buffer + m_offset + 40, src, sizeof(char) * 8);
        return *this;
    }

    SBE_NODISCARD std::string getCounterCurrencyAsString() const
    {
        const char *buffer = m_buffer + m_offset + 40;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getCounterCurrencyAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getCounterCurrencyAsString();

        for (const auto c : s)
        {
            switch (c)
            {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;

                default:
                    if ('\x00' <= c && c <= '\x1f')
                    {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    }
                    else
                    {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

    #if __cplusplus >= 201703L
    SBE_NODISCARD std::string_view getCounterCurrencyAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 40;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #if __cplusplus >= 201703L
    Instrument &putCounterCurrency(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putCounterCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 40, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 40 + start] = 0;
        }

        return *this;
    }
    #else
    Instrument &putCounterCurrency(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putCounterCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 40, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 40 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *settlementCurrencyMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t settlementCurrencyId() SBE_NOEXCEPT
    {
        return 13;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t settlementCurrencySinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool settlementCurrencyInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= settlementCurrencySinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t settlementCurrencyEncodingOffset() SBE_NOEXCEPT
    {
        return 48;
    }

    static SBE_CONSTEXPR char settlementCurrencyNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char settlementCurrencyMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char settlementCurrencyMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t settlementCurrencyEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t settlementCurrencyLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD const char *settlementCurrency() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 48;
    }

    SBE_NODISCARD char *settlementCurrency() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 48;
    }

    SBE_NODISCARD char settlementCurrency(const std::uint64_t index) const
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for settlementCurrency [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 48 + (index * 1), sizeof(char));
        return (val);
    }

    Instrument &settlementCurrency(const std::uint64_t index, const char value)
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for settlementCurrency [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 48 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getSettlementCurrency(char *const dst, const std::uint64_t length) const
    {
        if (length > 8)
        {
            throw std::runtime_error("length too large for getSettlementCurrency [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 48, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    Instrument &putSettlementCurrency(const char *const src) SBE_NOEXCEPT
    {
        std::memcpy(m_buffer + m_offset + 48, src, sizeof(char) * 8);
        return *this;
    }

    SBE_NODISCARD std::string getSettlementCurrencyAsString() const
    {
        const char *buffer = m_buffer + m_offset + 48;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getSettlementCurrencyAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getSettlementCurrencyAsString();

        for (const auto c : s)
        {
            switch (c)
            {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;

                default:
                    if ('\x00' <= c && c <= '\x1f')
                    {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    }
                    else
                    {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

    #if __cplusplus >= 201703L
    SBE_NODISCARD std::string_view getSettlementCurrencyAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 48;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #if __cplusplus >= 201703L
    Instrument &putSettlementCurrency(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putSettlementCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 48, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 48 + start] = 0;
        }

        return *this;
    }
    #else
    Instrument &putSettlementCurrency(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putSettlementCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 48, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 48 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *sizeCurrencyMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t sizeCurrencyId() SBE_NOEXCEPT
    {
        return 14;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t sizeCurrencySinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool sizeCurrencyInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= sizeCurrencySinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t sizeCurrencyEncodingOffset() SBE_NOEXCEPT
    {
        return 56;
    }

    static SBE_CONSTEXPR char sizeCurrencyNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char sizeCurrencyMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char sizeCurrencyMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t sizeCurrencyEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t sizeCurrencyLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD const char *sizeCurrency() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 56;
    }

    SBE_NODISCARD char *sizeCurrency() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 56;
    }

    SBE_NODISCARD char sizeCurrency(const std::uint64_t index) const
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for sizeCurrency [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 56 + (index * 1), sizeof(char));
        return (val);
    }

    Instrument &sizeCurrency(const std::uint64_t index, const char value)
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for sizeCurrency [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 56 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getSizeCurrency(char *const dst, const std::uint64_t length) const
    {
        if (length > 8)
        {
            throw std::runtime_error("length too large for getSizeCurrency [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 56, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    Instrument &putSizeCurrency(const char *const src) SBE_NOEXCEPT
    {
        std::memcpy(m_buffer + m_offset + 56, src, sizeof(char) * 8);
        return *this;
    }

    SBE_NODISCARD std::string getSizeCurrencyAsString() const
    {
        const char *buffer = m_buffer + m_offset + 56;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getSizeCurrencyAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getSizeCurrencyAsString();

        for (const auto c : s)
        {
            switch (c)
            {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;

                default:
                    if ('\x00' <= c && c <= '\x1f')
                    {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    }
                    else
                    {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

    #if __cplusplus >= 201703L
    SBE_NODISCARD std::string_view getSizeCurrencyAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 56;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #if __cplusplus >= 201703L
    Instrument &putSizeCurrency(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putSizeCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 56, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 56 + start] = 0;
        }

        return *this;
    }
    #else
    Instrument &putSizeCurrency(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putSizeCurrency [E106]");
        }

        std::memcpy(m_buffer + m_offset + 56, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 56 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *creationTimestampMsMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t creationTimestampMsId() SBE_NOEXCEPT
    {
        return 15;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t creationTimestampMsSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool creationTimestampMsInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= creationTimestampMsSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t creationTimestampMsEncodingOffset() SBE_NOEXCEPT
    {
        return 64;
    }

    static SBE_CONSTEXPR std::uint64_t creationTimestampMsNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT64;
    }

    static SBE_CONSTEXPR std::uint64_t creationTimestampMsMinValue() SBE_NOEXCEPT
    {
        return UINT64_C(0x0);
    }

    static SBE_CONSTEXPR std::uint64_t creationTimestampMsMaxValue() SBE_NOEXCEPT
    {
        return UINT64_C(0xfffffffffffffffe);
    }

    static SBE_CONSTEXPR std::size_t creationTimestampMsEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD std::uint64_t creationTimestampMs() const SBE_NOEXCEPT
    {
        std::uint64_t val;
        std::memcpy(&val, m_buffer + m_offset + 64, sizeof(std::uint64_t));
        return SBE_LITTLE_ENDIAN_ENCODE_64(val);
    }

    Instrument &creationTimestampMs(const std::uint64_t value) SBE_NOEXCEPT
    {
        std::uint64_t val = SBE_LITTLE_ENDIAN_ENCODE_64(value);
        std::memcpy(m_buffer + m_offset + 64, &val, sizeof(std::uint64_t));
        return *this;
    }

    SBE_NODISCARD static const char *expirationTimestampMsMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t expirationTimestampMsId() SBE_NOEXCEPT
    {
        return 16;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t expirationTimestampMsSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool expirationTimestampMsInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= expirationTimestampMsSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t expirationTimestampMsEncodingOffset() SBE_NOEXCEPT
    {
        return 72;
    }

    static SBE_CONSTEXPR std::uint64_t expirationTimestampMsNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT64;
    }

    static SBE_CONSTEXPR std::uint64_t expirationTimestampMsMinValue() SBE_NOEXCEPT
    {
        return UINT64_C(0x0);
    }

    static SBE_CONSTEXPR std::uint64_t expirationTimestampMsMaxValue() SBE_NOEXCEPT
    {
        return UINT64_C(0xfffffffffffffffe);
    }

    static SBE_CONSTEXPR std::size_t expirationTimestampMsEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD std::uint64_t expirationTimestampMs() const SBE_NOEXCEPT
    {
        std::uint64_t val;
        std::memcpy(&val, m_buffer + m_offset + 72, sizeof(std::uint64_t));
        return SBE_LITTLE_ENDIAN_ENCODE_64(val);
    }

    Instrument &expirationTimestampMs(const std::uint64_t value) SBE_NOEXCEPT
    {
        std::uint64_t val = SBE_LITTLE_ENDIAN_ENCODE_64(value);
        std::memcpy(m_buffer + m_offset + 72, &val, sizeof(std::uint64_t));
        return *this;
    }

    SBE_NODISCARD static const char *strikePriceMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t strikePriceId() SBE_NOEXCEPT
    {
        return 17;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t strikePriceSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool strikePriceInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= strikePriceSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t strikePriceEncodingOffset() SBE_NOEXCEPT
    {
        return 80;
    }

    static SBE_CONSTEXPR double strikePriceNullValue() SBE_NOEXCEPT
    {
        return SBE_DOUBLE_NAN;
    }

    static SBE_CONSTEXPR double strikePriceMinValue() SBE_NOEXCEPT
    {
        return 4.9E-324;
    }

    static SBE_CONSTEXPR double strikePriceMaxValue() SBE_NOEXCEPT
    {
        return 1.7976931348623157E308;
    }

    static SBE_CONSTEXPR std::size_t strikePriceEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD double strikePrice() const SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        std::memcpy(&val, m_buffer + m_offset + 80, sizeof(double));
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        return val.fp_value;
    }

    Instrument &strikePrice(const double value) SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        val.fp_value = value;
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        std::memcpy(m_buffer + m_offset + 80, &val, sizeof(double));
        return *this;
    }

    SBE_NODISCARD static const char *contractSizeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t contractSizeId() SBE_NOEXCEPT
    {
        return 18;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t contractSizeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool contractSizeInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= contractSizeSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t contractSizeEncodingOffset() SBE_NOEXCEPT
    {
        return 88;
    }

    static SBE_CONSTEXPR double contractSizeNullValue() SBE_NOEXCEPT
    {
        return SBE_DOUBLE_NAN;
    }

    static SBE_CONSTEXPR double contractSizeMinValue() SBE_NOEXCEPT
    {
        return 4.9E-324;
    }

    static SBE_CONSTEXPR double contractSizeMaxValue() SBE_NOEXCEPT
    {
        return 1.7976931348623157E308;
    }

    static SBE_CONSTEXPR std::size_t contractSizeEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD double contractSize() const SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        std::memcpy(&val, m_buffer + m_offset + 88, sizeof(double));
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        return val.fp_value;
    }

    Instrument &contractSize(const double value) SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        val.fp_value = value;
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        std::memcpy(m_buffer + m_offset + 88, &val, sizeof(double));
        return *this;
    }

    SBE_NODISCARD static const char *minTradeAmountMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t minTradeAmountId() SBE_NOEXCEPT
    {
        return 19;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t minTradeAmountSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool minTradeAmountInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= minTradeAmountSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t minTradeAmountEncodingOffset() SBE_NOEXCEPT
    {
        return 96;
    }

    static SBE_CONSTEXPR double minTradeAmountNullValue() SBE_NOEXCEPT
    {
        return SBE_DOUBLE_NAN;
    }

    static SBE_CONSTEXPR double minTradeAmountMinValue() SBE_NOEXCEPT
    {
        return 4.9E-324;
    }

    static SBE_CONSTEXPR double minTradeAmountMaxValue() SBE_NOEXCEPT
    {
        return 1.7976931348623157E308;
    }

    static SBE_CONSTEXPR std::size_t minTradeAmountEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD double minTradeAmount() const SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        std::memcpy(&val, m_buffer + m_offset + 96, sizeof(double));
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        return val.fp_value;
    }

    Instrument &minTradeAmount(const double value) SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        val.fp_value = value;
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        std::memcpy(m_buffer + m_offset + 96, &val, sizeof(double));
        return *this;
    }

    SBE_NODISCARD static const char *tickSizeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t tickSizeId() SBE_NOEXCEPT
    {
        return 20;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t tickSizeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool tickSizeInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= tickSizeSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t tickSizeEncodingOffset() SBE_NOEXCEPT
    {
        return 104;
    }

    static SBE_CONSTEXPR double tickSizeNullValue() SBE_NOEXCEPT
    {
        return SBE_DOUBLE_NAN;
    }

    static SBE_CONSTEXPR double tickSizeMinValue() SBE_NOEXCEPT
    {
        return 4.9E-324;
    }

    static SBE_CONSTEXPR double tickSizeMaxValue() SBE_NOEXCEPT
    {
        return 1.7976931348623157E308;
    }

    static SBE_CONSTEXPR std::size_t tickSizeEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD double tickSize() const SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        std::memcpy(&val, m_buffer + m_offset + 104, sizeof(double));
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        return val.fp_value;
    }

    Instrument &tickSize(const double value) SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        val.fp_value = value;
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        std::memcpy(m_buffer + m_offset + 104, &val, sizeof(double));
        return *this;
    }

    SBE_NODISCARD static const char *makerCommissionMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t makerCommissionId() SBE_NOEXCEPT
    {
        return 21;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t makerCommissionSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool makerCommissionInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= makerCommissionSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t makerCommissionEncodingOffset() SBE_NOEXCEPT
    {
        return 112;
    }

    static SBE_CONSTEXPR double makerCommissionNullValue() SBE_NOEXCEPT
    {
        return SBE_DOUBLE_NAN;
    }

    static SBE_CONSTEXPR double makerCommissionMinValue() SBE_NOEXCEPT
    {
        return 4.9E-324;
    }

    static SBE_CONSTEXPR double makerCommissionMaxValue() SBE_NOEXCEPT
    {
        return 1.7976931348623157E308;
    }

    static SBE_CONSTEXPR std::size_t makerCommissionEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD double makerCommission() const SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        std::memcpy(&val, m_buffer + m_offset + 112, sizeof(double));
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        return val.fp_value;
    }

    Instrument &makerCommission(const double value) SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        val.fp_value = value;
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        std::memcpy(m_buffer + m_offset + 112, &val, sizeof(double));
        return *this;
    }

    SBE_NODISCARD static const char *takerCommissionMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t takerCommissionId() SBE_NOEXCEPT
    {
        return 22;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t takerCommissionSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool takerCommissionInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= takerCommissionSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t takerCommissionEncodingOffset() SBE_NOEXCEPT
    {
        return 120;
    }

    static SBE_CONSTEXPR double takerCommissionNullValue() SBE_NOEXCEPT
    {
        return SBE_DOUBLE_NAN;
    }

    static SBE_CONSTEXPR double takerCommissionMinValue() SBE_NOEXCEPT
    {
        return 4.9E-324;
    }

    static SBE_CONSTEXPR double takerCommissionMaxValue() SBE_NOEXCEPT
    {
        return 1.7976931348623157E308;
    }

    static SBE_CONSTEXPR std::size_t takerCommissionEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD double takerCommission() const SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        std::memcpy(&val, m_buffer + m_offset + 120, sizeof(double));
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        return val.fp_value;
    }

    Instrument &takerCommission(const double value) SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        val.fp_value = value;
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        std::memcpy(m_buffer + m_offset + 120, &val, sizeof(double));
        return *this;
    }

    SBE_NODISCARD static const char *blockTradeCommissionMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t blockTradeCommissionId() SBE_NOEXCEPT
    {
        return 23;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t blockTradeCommissionSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool blockTradeCommissionInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= blockTradeCommissionSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t blockTradeCommissionEncodingOffset() SBE_NOEXCEPT
    {
        return 128;
    }

    static SBE_CONSTEXPR double blockTradeCommissionNullValue() SBE_NOEXCEPT
    {
        return SBE_DOUBLE_NAN;
    }

    static SBE_CONSTEXPR double blockTradeCommissionMinValue() SBE_NOEXCEPT
    {
        return 4.9E-324;
    }

    static SBE_CONSTEXPR double blockTradeCommissionMaxValue() SBE_NOEXCEPT
    {
        return 1.7976931348623157E308;
    }

    static SBE_CONSTEXPR std::size_t blockTradeCommissionEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD double blockTradeCommission() const SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        std::memcpy(&val, m_buffer + m_offset + 128, sizeof(double));
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        return val.fp_value;
    }

    Instrument &blockTradeCommission(const double value) SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        val.fp_value = value;
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        std::memcpy(m_buffer + m_offset + 128, &val, sizeof(double));
        return *this;
    }

    SBE_NODISCARD static const char *maxLiquidationCommissionMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t maxLiquidationCommissionId() SBE_NOEXCEPT
    {
        return 24;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t maxLiquidationCommissionSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool maxLiquidationCommissionInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= maxLiquidationCommissionSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t maxLiquidationCommissionEncodingOffset() SBE_NOEXCEPT
    {
        return 136;
    }

    static SBE_CONSTEXPR double maxLiquidationCommissionNullValue() SBE_NOEXCEPT
    {
        return SBE_DOUBLE_NAN;
    }

    static SBE_CONSTEXPR double maxLiquidationCommissionMinValue() SBE_NOEXCEPT
    {
        return 4.9E-324;
    }

    static SBE_CONSTEXPR double maxLiquidationCommissionMaxValue() SBE_NOEXCEPT
    {
        return 1.7976931348623157E308;
    }

    static SBE_CONSTEXPR std::size_t maxLiquidationCommissionEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD double maxLiquidationCommission() const SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        std::memcpy(&val, m_buffer + m_offset + 136, sizeof(double));
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        return val.fp_value;
    }

    Instrument &maxLiquidationCommission(const double value) SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        val.fp_value = value;
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        std::memcpy(m_buffer + m_offset + 136, &val, sizeof(double));
        return *this;
    }

    SBE_NODISCARD static const char *maxLeverageMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t maxLeverageId() SBE_NOEXCEPT
    {
        return 25;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t maxLeverageSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool maxLeverageInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= maxLeverageSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t maxLeverageEncodingOffset() SBE_NOEXCEPT
    {
        return 144;
    }

    static SBE_CONSTEXPR double maxLeverageNullValue() SBE_NOEXCEPT
    {
        return SBE_DOUBLE_NAN;
    }

    static SBE_CONSTEXPR double maxLeverageMinValue() SBE_NOEXCEPT
    {
        return 4.9E-324;
    }

    static SBE_CONSTEXPR double maxLeverageMaxValue() SBE_NOEXCEPT
    {
        return 1.7976931348623157E308;
    }

    static SBE_CONSTEXPR std::size_t maxLeverageEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD double maxLeverage() const SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        std::memcpy(&val, m_buffer + m_offset + 144, sizeof(double));
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        return val.fp_value;
    }

    Instrument &maxLeverage(const double value) SBE_NOEXCEPT
    {
        union sbe_double_as_uint_u val;
        val.fp_value = value;
        val.uint_value = SBE_LITTLE_ENDIAN_ENCODE_64(val.uint_value);
        std::memcpy(m_buffer + m_offset + 144, &val, sizeof(double));
        return *this;
    }

    SBE_NODISCARD static const char *instrumentNameMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static const char *instrumentNameCharacterEncoding() SBE_NOEXCEPT
    {
        return "null";
    }

    static SBE_CONSTEXPR std::uint64_t instrumentNameSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    bool instrumentNameInActingVersion() SBE_NOEXCEPT
    {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
        return m_actingVersion >= instrumentNameSinceVersion();
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }

    static SBE_CONSTEXPR std::uint16_t instrumentNameId() SBE_NOEXCEPT
    {
        return 26;
    }

    static SBE_CONSTEXPR std::uint64_t instrumentNameHeaderLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t instrumentNameLength() const
    {
        std::uint8_t length;
        std::memcpy(&length, m_buffer + sbePosition(), sizeof(std::uint8_t));
        return (length);
    }

    std::uint64_t skipInstrumentName()
    {
        std::uint64_t lengthOfLengthField = 1;
        std::uint64_t lengthPosition = sbePosition();
        std::uint8_t lengthFieldValue;
        std::memcpy(&lengthFieldValue, m_buffer + lengthPosition, sizeof(std::uint8_t));
        std::uint64_t dataLength = (lengthFieldValue);
        sbePosition(lengthPosition + lengthOfLengthField + dataLength);
        return dataLength;
    }

    SBE_NODISCARD const char *instrumentName()
    {
        std::uint8_t lengthFieldValue;
        std::memcpy(&lengthFieldValue, m_buffer + sbePosition(), sizeof(std::uint8_t));
        const char *fieldPtr = m_buffer + sbePosition() + 1;
        sbePosition(sbePosition() + 1 + (lengthFieldValue));
        return fieldPtr;
    }

    std::uint64_t getInstrumentName(char *dst, const std::uint64_t length)
    {
        std::uint64_t lengthOfLengthField = 1;
        std::uint64_t lengthPosition = sbePosition();
        sbePosition(lengthPosition + lengthOfLengthField);
        std::uint8_t lengthFieldValue;
        std::memcpy(&lengthFieldValue, m_buffer + lengthPosition, sizeof(std::uint8_t));
        std::uint64_t dataLength = (lengthFieldValue);
        std::uint64_t bytesToCopy = length < dataLength ? length : dataLength;
        std::uint64_t pos = sbePosition();
        sbePosition(pos + dataLength);
        std::memcpy(dst, m_buffer + pos, static_cast<std::size_t>(bytesToCopy));
        return bytesToCopy;
    }

    Instrument &putInstrumentName(const char *src, const std::uint8_t length)
    {
        std::uint64_t lengthOfLengthField = 1;
        std::uint64_t lengthPosition = sbePosition();
        std::uint8_t lengthFieldValue = (length);
        sbePosition(lengthPosition + lengthOfLengthField);
        std::memcpy(m_buffer + lengthPosition, &lengthFieldValue, sizeof(std::uint8_t));
        if (length != std::uint8_t(0))
        {
            std::uint64_t pos = sbePosition();
            sbePosition(pos + length);
            std::memcpy(m_buffer + pos, src, length);
        }
        return *this;
    }

    std::string getInstrumentNameAsString()
    {
        std::uint64_t lengthOfLengthField = 1;
        std::uint64_t lengthPosition = sbePosition();
        sbePosition(lengthPosition + lengthOfLengthField);
        std::uint8_t lengthFieldValue;
        std::memcpy(&lengthFieldValue, m_buffer + lengthPosition, sizeof(std::uint8_t));
        std::uint64_t dataLength = (lengthFieldValue);
        std::uint64_t pos = sbePosition();
        const std::string result(m_buffer + pos, dataLength);
        sbePosition(pos + dataLength);
        return result;
    }

    std::string getInstrumentNameAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getInstrumentNameAsString();

        for (const auto c : s)
        {
            switch (c)
            {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;

                default:
                    if ('\x00' <= c && c <= '\x1f')
                    {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    }
                    else
                    {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

    #if __cplusplus >= 201703L
    std::string_view getInstrumentNameAsStringView()
    {
        std::uint64_t lengthOfLengthField = 1;
        std::uint64_t lengthPosition = sbePosition();
        sbePosition(lengthPosition + lengthOfLengthField);
        std::uint8_t lengthFieldValue;
        std::memcpy(&lengthFieldValue, m_buffer + lengthPosition, sizeof(std::uint8_t));
        std::uint64_t dataLength = (lengthFieldValue);
        std::uint64_t pos = sbePosition();
        const std::string_view result(m_buffer + pos, dataLength);
        sbePosition(pos + dataLength);
        return result;
    }
    #endif

    Instrument &putInstrumentName(const std::string &str)
    {
        if (str.length() > 254)
        {
            throw std::runtime_error("std::string too long for length type [E109]");
        }
        return putInstrumentName(str.data(), static_cast<std::uint8_t>(str.length()));
    }

    #if __cplusplus >= 201703L
    Instrument &putInstrumentName(const std::string_view str)
    {
        if (str.length() > 254)
        {
            throw std::runtime_error("std::string too long for length type [E109]");
        }
        return putInstrumentName(str.data(), static_cast<std::uint8_t>(str.length()));
    }
    #endif

template<typename CharT, typename Traits>
friend std::basic_ostream<CharT, Traits> & operator << (
    std::basic_ostream<CharT, Traits> &builder, Instrument _writer)
{
    Instrument writer(
        _writer.m_buffer,
        _writer.m_offset,
        _writer.m_bufferLength,
        _writer.m_actingBlockLength,
        _writer.m_actingVersion);

    builder << '{';
    builder << R"("Name": "Instrument", )";
    builder << R"("sbeTemplateId": )";
    builder << writer.sbeTemplateId();
    builder << ", ";

    builder << R"("header": )";
    builder << writer.header();

    builder << ", ";
    builder << R"("instrumentId": )";
    builder << +writer.instrumentId();

    builder << ", ";
    builder << R"("instrumentState": )";
    builder << '"' << writer.instrumentState() << '"';

    builder << ", ";
    builder << R"("kind": )";
    builder << '"' << writer.kind() << '"';

    builder << ", ";
    builder << R"("futureType": )";
    builder << '"' << writer.futureType() << '"';

    builder << ", ";
    builder << R"("optionType": )";
    builder << '"' << writer.optionType() << '"';

    builder << ", ";
    builder << R"("rfq": )";
    builder << '"' << writer.rfq() << '"';

    builder << ", ";
    builder << R"("settlementPeriod": )";
    builder << '"' << writer.settlementPeriod() << '"';

    builder << ", ";
    builder << R"("settlementPeriodCount": )";
    builder << +writer.settlementPeriodCount();

    builder << ", ";
    builder << R"("baseCurrency": )";
    builder << '"' <<
        writer.getBaseCurrencyAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("quoteCurrency": )";
    builder << '"' <<
        writer.getQuoteCurrencyAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("counterCurrency": )";
    builder << '"' <<
        writer.getCounterCurrencyAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("settlementCurrency": )";
    builder << '"' <<
        writer.getSettlementCurrencyAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("sizeCurrency": )";
    builder << '"' <<
        writer.getSizeCurrencyAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("creationTimestampMs": )";
    builder << +writer.creationTimestampMs();

    builder << ", ";
    builder << R"("expirationTimestampMs": )";
    builder << +writer.expirationTimestampMs();

    builder << ", ";
    builder << R"("strikePrice": )";
    builder << +writer.strikePrice();

    builder << ", ";
    builder << R"("contractSize": )";
    builder << +writer.contractSize();

    builder << ", ";
    builder << R"("minTradeAmount": )";
    builder << +writer.minTradeAmount();

    builder << ", ";
    builder << R"("tickSize": )";
    builder << +writer.tickSize();

    builder << ", ";
    builder << R"("makerCommission": )";
    builder << +writer.makerCommission();

    builder << ", ";
    builder << R"("takerCommission": )";
    builder << +writer.takerCommission();

    builder << ", ";
    builder << R"("blockTradeCommission": )";
    builder << +writer.blockTradeCommission();

    builder << ", ";
    builder << R"("maxLiquidationCommission": )";
    builder << +writer.maxLiquidationCommission();

    builder << ", ";
    builder << R"("maxLeverage": )";
    builder << +writer.maxLeverage();

    builder << ", ";
    builder << R"("instrumentName": )";
    builder << '"' <<
        writer.skipInstrumentName() << " bytes of raw data\"";
    builder << '}';

    return builder;
}

void skip()
{
    skipInstrumentName();
}

SBE_NODISCARD static SBE_CONSTEXPR bool isConstLength() SBE_NOEXCEPT
{
    return false;
}

SBE_NODISCARD static std::size_t computeLength(std::size_t instrumentNameLength = 0)
{
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    std::size_t length = sbeBlockLength();

    length += instrumentNameHeaderLength();
    if (instrumentNameLength > 254LL)
    {
        throw std::runtime_error("instrumentNameLength too long for length type [E109]");
    }
    length += instrumentNameLength;

    return length;
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
}
};
}
#endif
