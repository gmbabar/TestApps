#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "WebSocketDecompressors.hpp"
#include <zlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/beast/core/flat_buffer.hpp> // flat_buffer
#include <chrono>


void MultiBufferWrite(boost::beast::multi_buffer &buffer, const std::string& data) {
	auto prepBuff = buffer.prepare(data.size());
	for (auto it : prepBuff) {
        memcpy(it.data(), data.c_str(), it.size());
	}
	buffer.commit(data.size());
}

void FlatBufferWrite(boost::beast::flat_buffer &buffer, const std::string& data) {
	auto fbuf = buffer.prepare(data.size());
	memcpy(static_cast<char*>(fbuf.data()), data.c_str(), data.size());
	buffer.commit(data.size());
}

std::string CompressToString(std::string plainString) {
    std::string compressedString;
    boost::iostreams::filtering_ostream oss;
    oss.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression)));
    oss.push(boost::iostreams::back_inserter(compressedString));
    oss << plainString;
    boost::iostreams::close(oss);
    return compressedString;
}

std::string CompressToStringZlib2(std::string plainString) {
    using namespace boost::iostreams::zlib;
    boost::iostreams::zlib_params params{default_compression, deflated, -MAX_WBITS, default_mem_level, default_strategy};

    std::string compressedString;
    boost::iostreams::filtering_ostream out;
    out.push(boost::iostreams::zlib_compressor(params));
    out.push(boost::iostreams::back_inserter(compressedString));
    out << plainString;
    boost::iostreams::close(out);
    return compressedString;
}

BOOST_AUTO_TEST_SUITE(Decompressor)

namespace {
    const std::string originalString = "The Quick Brown Fox Jumped Over The Lazy Dog";
}

BOOST_AUTO_TEST_CASE(Test_PlainDecompressor_Multibuffer)
{
    boost::beast::multi_buffer buffer;
    nebula::tr::ds::impl::WebSocketPlainDecompressor wsDecompressor;
    MultiBufferWrite(buffer, originalString);
    std::stringstream sstream;
    wsDecompressor.decompress(sstream, buffer);
    std::string decompressed = sstream.str();
    BOOST_REQUIRE_EQUAL(decompressed, originalString);
}

BOOST_AUTO_TEST_CASE(Test_PlainDecompressor_Flatbuffer)
{
    boost::beast::flat_buffer buffer;
    nebula::tr::ds::impl::WebSocketPlainDecompressor wsDecompressor;
    FlatBufferWrite(buffer, originalString);
    const char *begin{nullptr};
    const char *end{nullptr};
    wsDecompressor.decompress(buffer, begin, end);
    std::string decompressed = std::string(begin, end-begin);
    BOOST_REQUIRE_EQUAL(decompressed, originalString);
}

BOOST_AUTO_TEST_CASE(Test_WebSocketGzipDecompressor_Flatbuffer)
{
    boost::beast::flat_buffer buffer;
    nebula::tr::ds::impl::WebSocketGzipDecompressor wsDecompressor;
    std::string compressedString = CompressToString(originalString);
    FlatBufferWrite(buffer, compressedString);
    const char *begin{nullptr};
    const char *end{nullptr};
    wsDecompressor.decompress(buffer, begin, end);
    buffer.consume(compressedString.size());    // cleanup
    std::string decompressed = std::string(begin, end-begin);
    BOOST_REQUIRE_EQUAL(decompressed, originalString);

    // Let's run second iteration, with smaller string.
    std::string originalStr = "A shorter string";
    compressedString = CompressToString(originalStr);
    FlatBufferWrite(buffer, compressedString);
    begin = nullptr;
    end = nullptr;
    wsDecompressor.decompress(buffer, begin, end);
    buffer.consume(compressedString.size());    // cleanup
    decompressed = std::string(begin, end-begin);
    BOOST_REQUIRE_EQUAL(decompressed, originalStr);

    // Let's run third iteration, with original/bigger string.
    compressedString = CompressToString(originalString);
    FlatBufferWrite(buffer, compressedString);
    begin = nullptr;
    end = nullptr;
    wsDecompressor.decompress(buffer, begin, end);
    buffer.consume(compressedString.size());    // cleanup
    decompressed = std::string(begin, end-begin);
    BOOST_REQUIRE_EQUAL(decompressed, originalString);
}

BOOST_AUTO_TEST_CASE(Test_WebSocketHuobiGzipDecompressor_Multibuffer)
{
    boost::beast::multi_buffer buffer;
    nebula::tr::ds::impl::WebSocketHuobiGzipDecompressor wsDecompressor;
    std::string compressedString = CompressToString(originalString);
    MultiBufferWrite(buffer, compressedString);
    std::stringstream sstream;
    wsDecompressor.decompress(sstream, buffer);
    std::string decompressed = sstream.str();
    BOOST_REQUIRE_EQUAL(decompressed, originalString);
}

BOOST_AUTO_TEST_CASE(Test_WebSocketHuobiGzipDecompressor_Flatbuffer)
{
    boost::beast::flat_buffer buffer;
    nebula::tr::ds::impl::WebSocketHuobiGzipDecompressor wsDecompressor;
    std::string compressedString = CompressToString(originalString);
    FlatBufferWrite(buffer, compressedString);
    const char *begin{nullptr};
    const char *end{nullptr};
    wsDecompressor.decompress(buffer, begin, end);
    std::string decompressed = std::string(begin, end-begin);
    BOOST_REQUIRE_EQUAL(decompressed, originalString);
}

BOOST_AUTO_TEST_CASE(Test_WebSocketFastGzipDecompressor_Multibuffer)
{
    boost::beast::multi_buffer buffer;
    nebula::tr::ds::impl::WebSocketFastGzipDecompressor wsDecompressor;
    std::string compressedString = CompressToStringZlib2(originalString);
    MultiBufferWrite(buffer, compressedString);
    std::stringstream sstream;
    wsDecompressor.decompress(sstream, buffer);
    std::string decompressed = sstream.str();
    BOOST_REQUIRE_EQUAL(decompressed, originalString);
}

BOOST_AUTO_TEST_CASE(Test_WebSocketFastGzipDecompressor_Flatbuffer)
{
    boost::beast::flat_buffer buffer;
    nebula::tr::ds::impl::WebSocketFastGzipDecompressor wsDecompressor;
    std::string compressedString = CompressToStringZlib2(originalString);
    FlatBufferWrite(buffer, compressedString);
    const char *begin{nullptr};
    const char *end{nullptr};
    wsDecompressor.decompress(buffer, begin, end);
    std::string decompressed = std::string(begin, end-begin);
    BOOST_REQUIRE_EQUAL(decompressed, originalString);
}

BOOST_AUTO_TEST_SUITE_END();
