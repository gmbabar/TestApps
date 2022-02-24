// https://stackoverflow.com/questions/20100059/boost-gzip-compressor-and-gzip-decompressor/20143445

#include <zlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/iostreams/copy.hpp>
// #include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/beast/core/flat_buffer.hpp> // flat_buffer
#include <chrono>

void TestOne(bool debug = true) {
    std::string stringData = "quick brown fox jumps over the lazy dog";

    // Compression
    std::string compressedString;
    {
        boost::iostreams::filtering_ostream oss;
        oss.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression)));
        oss.push(boost::iostreams::back_inserter(compressedString));
        oss << stringData;
        boost::iostreams::close(oss);
    }
    if (debug) {
        std::cout << "Pre-compression: " << stringData << std::endl;
        std::cout << "Postcompression: " << compressedString << std::endl;
    }

    // Decompression 
    std::string decompressedString;
    {
        boost::iostreams::filtering_ostream out;
        out.push(boost::iostreams::gzip_decompressor());
        out.push(boost::iostreams::back_inserter(decompressedString));
        out << compressedString;
        boost::iostreams::close(out);
    }
    if (debug) {
        std::cout << "Decompressed: " << decompressedString << std::endl;
        std::cout << std::endl;
    }
}

void TestTwo(bool debug = true) {
    std::string stringData = "quick brown fox jumps over the lazy dog";

    // Compression
    std::string compressedString;
    {
        boost::iostreams::filtering_ostream out;
        out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression)));
        out.push(boost::iostreams::back_inserter(compressedString));
        out << stringData;
        boost::iostreams::close(out);
    }
    if (debug) {
        std::cout << "Pre-compression: " << stringData << std::endl;
        std::cout << "Postcompression: " << compressedString << std::endl;
    }

    // Decompression 
    std::string decompressedString;
    {
        std::stringstream gss, ss;
        gss << compressedString;
        boost::iostreams::filtering_streambuf <boost::iostreams::output> out(ss);
        boost::iostreams::filtering_streambuf <boost::iostreams::input> in;
        in.push(boost::iostreams::gzip_decompressor());
        in.push(gss);
        boost::iostreams::copy(in, out);
        decompressedString = ss.str();
    }
    if (debug) {
        std::cout << "Decompressed: " << decompressedString << std::endl;
        std::cout << std::endl;
    }
}

void TestThree(bool debug = true) {
    std::string stringData = "quick brown fox jumps over the lazy dog";

    // Compression
    std::string compressedString;
    {
        boost::iostreams::filtering_ostream out;
        out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression)));
        out.push(boost::iostreams::back_inserter(compressedString));
        out << stringData;
        boost::iostreams::close(out);
    }
    if (debug) {
        std::cout << "Pre-compression: " << stringData << std::endl;
        std::cout << "Postcompression: " << compressedString << std::endl;
    }

    // Decompression 
    std::string decompressedString;
    {    
        // Simulate actual decompressor code.
        boost::beast::flat_buffer aBuffer(1024);
        auto fbuf = aBuffer.prepare(compressedString.size());
        memcpy(static_cast<char*>(fbuf.data()), compressedString.c_str(), compressedString.size());
        aBuffer.commit(compressedString.size());
        std::cout << "Commit " << compressedString.size() << " bytes, fbuf size: " << fbuf.size() << ", buffer size: " << aBuffer.size() << std::endl;

        // now decompress
        std::stringstream gss, ss;
        gss << boost::beast::buffers_to_string(aBuffer.data());
        boost::iostreams::filtering_streambuf <boost::iostreams::output> out(ss);
        boost::iostreams::filtering_streambuf <boost::iostreams::input> in;
        in.push(boost::iostreams::gzip_decompressor());
        in.push(gss);
        boost::iostreams::copy(in, out);

        {
        boost::iostreams::filtering_ostream out;
        out.push(boost::iostreams::gzip_decompressor());
        out.push(boost::iostreams::back_inserter(decompressedString));
        out << boost::beast::buffers_to_string(aBuffer.data());
        boost::iostreams::close(out);
        }

        aBuffer.consume(fbuf.size());        // This order works.

        // Now push back on flat_buffer.
        const std::string &refStr = ss.str();        
        auto abuf = aBuffer.prepare(refStr.size());
        memcpy(static_cast<char*>(abuf.data()), refStr.c_str(), refStr.size());
        aBuffer.commit(abuf.size());        // Don't call consume while prepare/commit in progress
        // aBuffer.consume(fbuf.size());    // This order works
        // aBuffer.commit(abuf.size());    // FAILED: order of consume then commit.

        // Now validate loaded data.
        if (debug) {
            // std::cout << "Decompressed-stream: " << ss.str() << std::endl;
            std::cout << "Decompressed-string: " << std::string(static_cast<const char*>(abuf.data()), abuf.size()) << std::endl;
            std::cout << "Decompressed-string: " << std::string(static_cast<const char*>(aBuffer.cdata().data()), aBuffer.cdata().size()) << std::endl;
            std::cout << std::endl;
        }
    }
}


void TestFour(bool debug = true) {
    std::string stringData = "quick brown fox jumps over the lazy dog";

    // Compression
    std::string compressedString;
    {
        boost::iostreams::filtering_ostream out;
        out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression)));
        out.push(boost::iostreams::back_inserter(compressedString));
        out << stringData;
        boost::iostreams::close(out);
    }
    if (debug) {
        std::cout << "Pre-compression: " << stringData << std::endl;
        std::cout << "Postcompression: " << compressedString << std::endl;
    }

    // Decompression 
    std::string decompressed;
    {    
        // Simulate actual decompressor code.
        boost::beast::flat_buffer aBuffer(1024);
        auto fbuf = aBuffer.prepare(compressedString.size());
        memcpy(static_cast<char*>(fbuf.data()), compressedString.c_str(), compressedString.size());
        aBuffer.commit(compressedString.size());

        // Now decompress to string
        boost::iostreams::filtering_ostream out;
        out.push(boost::iostreams::gzip_decompressor());
        out.push(boost::iostreams::back_inserter(decompressed));
        out << boost::beast::buffers_to_string(aBuffer.data());
        boost::iostreams::close(out);

        aBuffer.consume(fbuf.size());        // This order works.

        // Now push back on flat_buffer.
        auto abuf = aBuffer.prepare(decompressed.size());
        memcpy(static_cast<char*>(abuf.data()), decompressed.c_str(), decompressed.size());
        aBuffer.commit(abuf.size());        // Don't call consume while prepare/commit in progress

        // Now validate loaded data.
        if (debug) {
            // std::cout << "Decompressed-stream: " << ss.str() << std::endl;
            std::cout << "Decompressed-string: " << std::string(static_cast<const char*>(abuf.data()), abuf.size()) << std::endl;
            std::cout << "Decompressed-string: " << std::string(static_cast<const char*>(aBuffer.cdata().data()), aBuffer.cdata().size()) << std::endl;
            std::cout << std::endl;
        }
    }
}

int main () {
    unsigned count = 10000;
    auto t1 = std::chrono::steady_clock::now();
    for (int i=0; i<count; ++i)
        TestOne(false);
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "Time taken by TestOne: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()/count << " nsec" << std::endl;
    // std::cout << std::endl;

    t1 = std::chrono::steady_clock::now();
    for (int i=0; i<count; ++i)
        TestTwo(false);
    t2 = std::chrono::steady_clock::now();
    std::cout << "Time taken by TestTwo: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()/count << " nsec" << std::endl;

    TestThree();
    TestFour();
}

