#ifndef FIXPARSERS_HPP
#define FIXPARSERS_HPP

#include <nebula/common.hpp>
#include <nebula/data/BufRange.hpp>
#include <nebula/dtm/DateTimeTypes.hpp>

inline nebula::dtm::ptime parseFixTs(const nebula::data::ConstBufRange &aBufRange) {
    // 20180724-08:37:28
    // 20180724-08:37:28.695
    // 20180724-08:37:28.695123
    if(nebula_unlikely(!aBufRange.valid()))
        throw std::invalid_argument("Failed to parse fix timestamp: Invalid buffer");
    auto size = aBufRange.size();
    if(size != 17 && size != 21 && size != 24) {
        std::stringstream ss;
        ss << "Invalid size for fix timestamp parsing: {" << size << "} " << aBufRange;
        throw std::invalid_argument(ss.str());
    }
    const char *buf = aBufRange.buf;
    const char *bufEnd = buf + 4;
    auto year = nebula::io::NumParse::as<unsigned long>(&buf, bufEnd);
    bufEnd = buf + 2;
    while (*buf == '0')
        ++buf;
    auto month = nebula::io::NumParse::as<unsigned long>(&buf, bufEnd);
    bufEnd = buf + 2;
    while (*buf == '0')
        ++buf;
    auto day = nebula::io::NumParse::as<unsigned long>(&buf, bufEnd);
    buf += 2;
    auto tsStr = aBufRange.asStr().substr(9);
    auto dtm = nebula::dtm::duration_from_string(tsStr);
    return nebula::dtm::ptime(boost::gregorian::date(year, month, day), dtm);
}
inline nebula::dtm::time_duration parseFixValidUntilDuration(const nebula::data::ConstBufRange &aBufRange) {
    if(nebula_unlikely(!aBufRange.valid()))
        throw std::invalid_argument("Failed to parse fix valid-until duration: Invalid buffer");
    auto size = aBufRange.size();
    if(size != 17 && size != 21 && size != 24) {
        std::stringstream ss;
        ss << "Invalid size for fix timestamp parsing: {" << size << "} " << aBufRange;
        throw std::invalid_argument(ss.str());
    }
    auto tsStr = aBufRange.asStr().substr(9);
    return nebula::dtm::duration_from_string(tsStr);
}
inline unsigned long parseFixULong(const nebula::data::ConstBufRange &aBufRange) {
    if(nebula_unlikely(!aBufRange.valid()))
        throw std::invalid_argument("Failed to parse fix unsigned long: Invalid buffer");
    const char *buf = aBufRange.buf;
    return nebula::io::NumParse::as<unsigned long>(&buf, aBufRange.bufEnd);
}
inline unsigned long parseFixNormalQty(const nebula::data::ConstBufRange &aBufRange) {
    return parseFixULong(aBufRange);
}
inline unsigned long parseFixCryptoQty(const nebula::data::ConstBufRange &aBufRange) {
    if(nebula_unlikely(!aBufRange.valid()))
        throw std::invalid_argument("Failed to parse fix crypto qty: Invalid buffer");
    return nebula::parseNebulaCryptoFromString(aBufRange.buf, aBufRange.bufEnd);
}
inline unsigned long parseFixQty(const nebula::gw::ExchContract &aExchContract,
        const nebula::data::ConstBufRange &aBufRange) {
    if(aExchContract.qtyMultiplier == 1'000'000'000)
        return parseFixCryptoQty(aBufRange);
    else
        return parseFixNormalQty(aBufRange);
}
inline double parseFixDouble(const nebula::data::ConstBufRange &aBufRange) {
    if(nebula_unlikely(!aBufRange.valid()))
        throw std::invalid_argument("Failed to parse fix double: Invalid buffer");
    const char *buf = aBufRange.buf;
    return nebula::io::NumParse::as<double>(&buf, aBufRange.bufEnd);
}
inline nebula::tr::Side parseFixSide(const nebula::data::ConstBufRange &aBufRange) {
    if(nebula_unlikely(!aBufRange.valid()))
        throw std::invalid_argument("Failed to parse fix side: Invalid buffer");
    switch (*aBufRange.buf) {
    case '1':
        return nebula::tr::Side::buy;
    case '2':
        return nebula::tr::Side::sell;
    };
    throw std::invalid_argument("Unknown fix side!");
}
inline std::string parseFixString(const nebula::data::ConstBufRange &aBufRange) {
    if(nebula_unlikely(!aBufRange.valid()))
        throw std::invalid_argument("Failed to parse fix sym ep: Invalid buffer");
    return std::string(aBufRange.buf, aBufRange.bufEnd - aBufRange.buf);
}

#endif /* FIXPARSERS_HPP */
