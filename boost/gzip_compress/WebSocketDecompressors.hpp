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

#ifndef NEBULA_TR_DS_IMPL_WEBSOCKETDECOMPRESSORS_HPP
#define NEBULA_TR_DS_IMPL_WEBSOCKETDECOMPRESSORS_HPP

#include <zlib.h>
#include <string>
#include <sstream>
//#include <nebula/common.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <iostream>

namespace nebula { namespace tr { namespace ds { namespace impl {

#define NEBULA_DECL 
#define nebula_likely(x)       __builtin_expect((x),1)
#define nebula_unlikely(x)     __builtin_expect((x),0)

// For WebSocket streams with no compression
struct NEBULA_DECL WebSocketPlainDecompressor {
    void decompress(
            std::stringstream &ss, 
            const boost::beast::multi_buffer &aBuffers) {
#if 0
        ss << boost::beast::buffers(aBuffers.data());
#endif
        for(const auto &dataBuf : aBuffers.data()) {
            const char * dataBufBegin = (const char*) dataBuf.data();
            const auto dataBufLength = dataBuf.size();
            ss.write(dataBufBegin, dataBufLength);
        }
    }

    void decompress(
            const boost::beast::flat_buffer &aBuffer,
            const char **dataBufBegin, 
            const char **dataBufEnd) {
        dataBufBegin = (const char*) aBuffer.cdata().data();
        dataBufEnd = (dataBufBegin + aBuffer.cdata().size());
    }

    void reset() {}
};

template<typename Container>
class gzip_stream_writer {
public:
    typedef typename Container::value_type  char_type;
    typedef boost::iostreams::sink_tag      category;
    gzip_stream_writer(Container& cnt) : container(&cnt) { }
    std::streamsize write(const char_type* s, std::streamsize n)
    { 
        container->assign(s, n); 
        return n;
    }
protected:
    Container* container;
};

// For WebSocket streams with gzip compression (HUBI)
struct NEBULA_DECL WebSocketGzipDecompressor {
    std::string decompressed;

    WebSocketGzipDecompressor() : decompressed(1024*100, 0) {}

    // void decompress(
    //         std::stringstream &ss, 
    //         const boost::beast::multi_buffer &aBuffers) {
    //     std::stringstream gss;
    //     gss << boost::beast::buffers(aBuffers.data());
    //     boost::iostreams::filtering_streambuf <boost::iostreams::output> out(ss);
    //     boost::iostreams::filtering_streambuf <boost::iostreams::input> in;
    //     in.push(boost::iostreams::gzip_decompressor());
    //     in.push(gss);
    //     boost::iostreams::copy(in, out);
    // }

    // Not actively being used though
    void decompress(
            const boost::beast::flat_buffer &aBuffer,
            const char ** dataBufBegin, 
            const char ** dataBufEnd) {
        // decompress.
		boost::iostreams::filtering_ostream out;
		out.push(boost::iostreams::gzip_decompressor());
		out.push(nebula::tr::ds::impl::gzip_stream_writer<std::string>(decompressed));
		out << boost::beast::buffers_to_string(aBuffer.data());
		boost::iostreams::close(out);

        dataBufBegin = decompressed.data();
        dataBufEnd = dataBufBegin + decompressed.size();
    }

    void reset() {}
};

// For WebSocket streams with weird gzip compression (OKEX)
struct NEBULA_DECL WebSocketHuobiGzipDecompressor {
    uint8_t uncompData[100000];
    uint8_t * const uncompBufBegin;
    const uint8_t * const uncompBufEnd;

    int err = 0;
    z_stream d_stream = {0};        /* decompression stream */

    WebSocketHuobiGzipDecompressor()
        : uncompData(),
        uncompBufBegin(&uncompData[0]),
        uncompBufEnd(uncompBufBegin + sizeof(uncompData) - 1) {
        // Initialize libz 
        d_stream.zalloc = NULL;
        d_stream.zfree = NULL;
        d_stream.opaque = NULL;
        d_stream.total_out = 0;

        if(inflateInit2(&d_stream, 15 + 32) != Z_OK) {
            std::cerr << "inflateInit error" << std::endl;
            return;
        }
    }
    ~WebSocketHuobiGzipDecompressor() {
        if(inflateEnd(&d_stream) != Z_OK) {
            std::cerr << "inflate end error" << std::endl;
            return;
        }
    }

    void decompress(
            std::stringstream &ss, 
            const boost::beast::multi_buffer &aBuffers) {
        // Reset libz
        if(nebula_unlikely(inflateReset2(&d_stream, 15 + 32) != Z_OK)) {
            std::cerr << "inflateInit error" << std::endl;
            return;
        }

        uint8_t * uncompBuf = uncompBufBegin;
        for(const auto &dataBuf : aBuffers.data()) {
            uint32_t uncompDataLength = uncompBufEnd - uncompBuf;
            const uint8_t * dataBufBegin = (const uint8_t*) dataBuf.data();
            const auto dataBufLength = dataBuf.size();
            
            d_stream.next_in = (Bytef*)dataBufBegin;
            d_stream.next_out = uncompBuf;
            d_stream.avail_in = dataBufLength;
            d_stream.avail_out = uncompDataLength;
            if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) {
                uncompBuf = uncompBufBegin + d_stream.total_out;
                break;
            }

            if(nebula_unlikely(err != Z_OK)) {
                std::cerr << "inflate error: " << d_stream.msg << std::endl;
                break;
            }

            uncompBuf = uncompBufBegin + d_stream.total_out;
        }

        // Write decompressed string to channel
        const uint32_t uncompDataLength = uncompBuf - uncompBufBegin;
        ss.write((const char*) uncompBufBegin, uncompDataLength);
    }

    void decompress(
            const boost::beast::flat_buffer &aBuffer,
            const char ** dataBuffBegin, 
            const char ** dataBuffEnd) {
        // Reset libz
        if(nebula_unlikely(inflateReset2(&d_stream, 15 + 32) != Z_OK)) {
            std::cerr << "inflateInit error" << std::endl;
            return;
        }

        uint8_t * uncompBuf = uncompBufBegin;
        const auto &dataBuf = aBuffer.cdata();
        uint32_t uncompDataLength = uncompBufEnd - uncompBuf;
        const uint8_t * dataBufBegin = (const uint8_t*) dataBuf.data();
        const auto dataBufLength = dataBuf.size();
        
        d_stream.next_in = (Bytef*)dataBufBegin;
        d_stream.next_out = uncompBuf;
        d_stream.avail_in = dataBufLength;
        d_stream.avail_out = uncompDataLength;
        err = inflate(&d_stream, Z_NO_FLUSH);
        if(nebula_unlikely(err != Z_STREAM_END && err != Z_OK)) {
            std::cerr << "inflate error: " << d_stream.msg << std::endl;
            return;
        }
        uncompBuf = uncompBufBegin + d_stream.total_out;

        // set ptrs to decompressed string 
        dataBuffBegin = (const char *)(uncompBufBegin);
        dataBuffEnd = (const char *)(uncompBuf);
    }

    void reset() {
        if(inflateEnd(&d_stream) != Z_OK) {
            std::cerr << "inflate end error" << std::endl;
            return;
        }
        d_stream.zalloc = NULL;
        d_stream.zfree = NULL;
        d_stream.opaque = NULL;
        d_stream.total_out = 0;
        if(inflateInit2(&d_stream, 15 + 32) != Z_OK) {
            std::cerr << "inflateInit error" << std::endl;
            return;
        }
    }
};

#if 0
// For WebSocket streams with weird gzip compression (OKEX)
struct NEBULA_DECL WebSocketOkexGzipDecompressor {
    uint8_t uncompData[100000];

    void decompress(
            std::stringstream &ss, 
            const boost::beast::multi_buffer &aBuffers) {
        std::stringstream gss;
        gss << boost::beast::buffers(aBuffers.data());
        const auto &msg = gss.str();

        // From DMA:Gateways/OKEXFutures/OKEXRestDataHandler.cpp
        uint32_t uncompDataLength = sizeof(uncompData) - 1;  
        gzdecompress(msg, msg.length(), &uncompData[0], &uncompDataLength);

        // we have seen on the price side that the gzdecompress algorithm provided by exchange sometimes does NOT have the 
        // tradiling '}' so any Json read will cause issue. So just check and add. 
        // Please Note : means that even for pong we add - this we may want to correct 
        // TODO - check whether we need this 
        if(uncompData[uncompDataLength - 1 ] != '}') {
            uncompData[uncompDataLength ] = '}';
            uncompDataLength++;        
        }

        // Write decompressed string to channel
        ss.write((char*)(&uncompData[0]), uncompDataLength);
    }

    int gzdecompress(const std::string& zdataStr, uint32_t nzdata, uint8_t *data, uint32_t *ndata) {
        int err = 0;
        z_stream d_stream = {0};        /* decompression stream */

        static char dummy_head[2] = {
            0x8 + 0x7 * 0x10,
            (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
        };

        d_stream.zalloc = NULL;
        d_stream.zfree = NULL;
        d_stream.opaque = NULL;
        d_stream.next_in = (Bytef*)zdataStr.data();
    //  d_stream.next_in = zdata;
        d_stream.avail_in = 0;
        d_stream.next_out = data;

        if (inflateInit2(&d_stream, -MAX_WBITS) != Z_OK) {
            return -1;
        }

        while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) 
        {
            d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
            if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END)
                break;

            if (err != Z_OK) {
                if (err == Z_DATA_ERROR) {
                    d_stream.next_in = (Bytef*) dummy_head;
                    d_stream.avail_in = sizeof(dummy_head);
                    if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
                        return -1;
                    }
                } else {
                    return -1;
                }
            }
        }

        if (inflateEnd(&d_stream)!= Z_OK)
            return -1;
        *ndata = d_stream.total_out;
        return 0;
    }    
};
#endif

// For WebSocket streams with weird gzip compression (OKEX)
struct NEBULA_DECL WebSocketFastGzipDecompressor {
    uint8_t uncompData[100000];
    uint8_t * const uncompBufBegin;
    const uint8_t * const uncompBufEnd;

    int err = 0;
    z_stream d_stream = {0};        /* decompression stream */

    WebSocketFastGzipDecompressor()
        : uncompData(),
        uncompBufBegin(&uncompData[0]),
        uncompBufEnd(uncompBufBegin + sizeof(uncompData) - 1) {
        // Initialize libz 
        d_stream.zalloc = NULL;
        d_stream.zfree = NULL;
        d_stream.opaque = NULL;
        d_stream.total_out = 0;

        if(inflateInit2(&d_stream, -MAX_WBITS) != Z_OK) {
            std::cerr << "inflateInit error" << std::endl;
            return;
        }
    }
    ~WebSocketFastGzipDecompressor() {
        if(inflateEnd(&d_stream) != Z_OK) {
            std::cerr << "inflate end error" << std::endl;
            return;
        }
    }

    void decompress(
            std::stringstream &ss, 
            const boost::beast::multi_buffer &aBuffers) {
        // Reset libz
        if(nebula_unlikely(inflateReset2(&d_stream, -MAX_WBITS) != Z_OK)) {
            std::cerr << "inflateInit error" << std::endl;
            return;
        }

        uint8_t * uncompBuf = uncompBufBegin;
        for(const auto &dataBuf : aBuffers.data()) {
            uint32_t uncompDataLength = uncompBufEnd - uncompBuf;
            const uint8_t * dataBufBegin = (const uint8_t*) dataBuf.data();
            const auto dataBufLength = dataBuf.size();
            
            d_stream.next_in = (Bytef*)dataBufBegin;
            d_stream.next_out = uncompBuf;
            d_stream.avail_in = dataBufLength;
            d_stream.avail_out = uncompDataLength;
            if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) {
                uncompBuf = uncompBufBegin + d_stream.total_out;
                break;
            }

            if(nebula_unlikely(err != Z_OK)) {
                std::cerr << "inflate error: " << d_stream.msg << std::endl;
                break;
            }

            uncompBuf = uncompBufBegin + d_stream.total_out;
        }

        // Write decompressed string to channel
        const uint32_t uncompDataLength = uncompBuf - uncompBufBegin;
        ss.write((const char*) uncompBufBegin, uncompDataLength);
    }
    
    void decompress(
            const boost::beast::flat_buffer &aBuffer,
            const char ** dataBuffBegin, 
            const char ** dataBuffEnd) {
        // Reset libz
        if(nebula_unlikely(inflateReset2(&d_stream, -MAX_WBITS) != Z_OK)) {
            std::cerr << "inflateInit error" << std::endl;
            return;
        }

        uint8_t * uncompBuf = uncompBufBegin;
        const auto &dataBuf = aBuffer.cdata();
        uint32_t uncompDataLength = uncompBufEnd - uncompBuf;
        const uint8_t * dataBufBegin = (const uint8_t*) dataBuf.data();
        const auto dataBufLength = dataBuf.size();
        
        d_stream.next_in = (Bytef*)dataBufBegin;
        d_stream.next_out = uncompBuf;
        d_stream.avail_in = dataBufLength;
        d_stream.avail_out = uncompDataLength;
        err = inflate(&d_stream, Z_NO_FLUSH);
        if(nebula_unlikely(err != Z_STREAM_END && err != Z_OK)) {
            std::cerr << "inflate error: " << d_stream.msg << std::endl;
            return;
        }
        uncompBuf = uncompBufBegin + d_stream.total_out;

        // set ptrs to decompressed string 
        dataBuffBegin = (const char *)uncompBufBegin;
        dataBuffEnd = (const char *)uncompBuf;
    }
    
    void reset() {
        if(inflateEnd(&d_stream) != Z_OK) {
            std::cerr << "inflate end error" << std::endl;
            return;
        }
        d_stream.zalloc = NULL;
        d_stream.zfree = NULL;
        d_stream.opaque = NULL;
        d_stream.total_out = 0;
        if(inflateInit2(&d_stream, -MAX_WBITS) != Z_OK) {
            std::cerr << "inflateInit error" << std::endl;
            return;
        }
    }
};

//typedef WebSocketFastGzipDecompressor WebSocketGzipDecompressor;
typedef WebSocketFastGzipDecompressor WebSocketOkexGzipDecompressor;
typedef WebSocketFastGzipDecompressor WebSocketOkexFastGzipDecompressor;

}}}}

#endif /* NEBULA_TR_DS_IMPL_WEBSOCKETDECOMPRESSORS_HPP */
