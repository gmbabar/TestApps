// TODO:
// 1. Fix formatting in both boost example as well as yours -done
// 2. Take command line argument for symbol to be subscribed -done
// 3. No message without type -done
// 4. Write another set of function to parse data using simple string parsing
//    [look at Bitfinex codebase, fastParseWs()]
//    NOTE: Only parse, L2Update (book), Trade/Ticker -done
//      Snapshot -left
//    - Try to receive snapshop data only once. (do it on last) -left

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <cppcodec/hex_lower.hpp>
#include <thread>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "ParseMessages.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace rapidjson;

//------------------------------------------------------------------------------

const std::string aPubKey = "47dfc129c1135efb2348d443297aaa0c";
const std::string aPriKey = "igIjLcJHQaBCMaYpGoexS2/HQ5jdKjXX7i4qsSAJkdcjb+deOkNHf0LfD9VDg9lGM+jfK5B6zSmyEASB0PYz1A==";
const std::string aPassPhrase = "bdtvyfvpyu9";

inline uint64_t gdaxTimestamp() {
    const auto p1 = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
}

inline std::string coinbaseSignature(const uint64_t &timestamp) {


    auto nonce = timestamp;
    // auto target = aRequest.target();

    std::stringstream ss;
    ss << nonce
       << boost::beast::http::verb::get
       << "/users/self/verify";
    const auto payload = ss.str();
    
    // DMA: Gateways/Coinbase/rest_api.cpp
    // Calculate signature
    char buffer[1024];
    auto msgSize = boost::beast::detail::base64::decode(buffer, aPriKey.c_str(), aPriKey.length()).first;
	unsigned int len = SHA256_DIGEST_LENGTH;
	unsigned char hash[SHA256_DIGEST_LENGTH];
#if OPENSSL_VERSION_NUMBER >= 0x1010100fL
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac, reinterpret_cast<const unsigned char*>(buffer), msgSize, EVP_sha256(), NULL);
    HMAC_Update(hmac, reinterpret_cast<const unsigned char*>(payload.c_str()), payload.length());
    HMAC_Final(hmac, hash, &len);
    HMAC_CTX_free(hmac);
#else
    HMAC_CTX hmac;
    HMAC_CTX_init(&hmac);
    HMAC_Init_ex(&hmac, reinterpret_cast<const unsigned char*>(buffer), size, EVP_sha256(), NULL);
    HMAC_Update(&hmac, reinterpret_cast<const unsigned char*>(payload.c_str()), payload.length());
    HMAC_Final(&hmac, hash, &len);
    HMAC_CTX_cleanup(&hmac);
#endif

    // Format into string
auto signature = cppcodec::hex_lower::encode(hash, len);
    // std::memset(buffer, 0, sizeof(buffer));
    msgSize = boost::beast::detail::base64::encode(buffer, hash, len);

std::cout << "Payload: " << payload << std::endl;
std::cout << "signature: " << signature << std::endl;
std::cout << "signature: " << std::string(buffer, msgSize) << std::endl;

    return std::string(buffer, msgSize);
}


std::string fmtGdaxSubscribe(const std::string &symbols) {

    // oss << "{\"type\":\"subscribe\",\"product_ids\":[" << argv[1] << "],\"channels\":[\"level2\",{\"name\":\"ticker\",\"product_ids\":[" << argv[1] << "]}], "signature":}";
    const auto timestamp = gdaxTimestamp();
    const auto signature = coinbaseSignature(timestamp);
    std::ostringstream oss;
    oss << "{"
        << R"("type":"subscribe")"
        << R"(, "product_ids": [)" << symbols << ']'
        << R"(, "channels": ["level2", {"name":"ticker", "product_ids": [)" << symbols << "]}]"
        << R"(, "signature": ")" << signature << '"'
        << R"(, "key": ")" << aPubKey << '"'
        << R"(, "passphrase": ")" << aPassPhrase << '"'
        << R"(, "timestamp": ")" << timestamp << '"'
        << "}";

    return oss.str();
}



// Report a failure
void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

// Sends a WebSocket message and prints the response
class Session : public std::enable_shared_from_this<Session> {


public:
    // Resolver and socket require an io_context
    explicit Session(net::io_context& ioc, ssl::context& ctx)
        : m_resolver(net::make_strand(ioc))
        , m_ws(net::make_strand(ioc), ctx) {
    }

    // Start the asynchronous operation
    void run( char const* host, char const* port, char const* text) {
        std::cout << "Listener:" << __func__ << std::endl;
        // Save these for later
        m_host = host;
        m_text = text;

        // Look up the domain name
        m_resolver.async_resolve(host,port,beast::bind_front_handler(&Session::on_resolve, shared_from_this()));
    }

    void on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "resolve");

        // Set a timeout on the operation
        beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(m_ws).async_connect(results, beast::bind_front_handler(&Session::on_connect, shared_from_this()));
    }

    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "connect");

        // Set a timeout on the operation
        beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(m_ws.next_layer().native_handle(), m_host.c_str())) {
            ec = beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
            return fail(ec, "connect");
        }

        // Update the m_host string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        m_host += ':' + std::to_string(ep.port());
        
        // Perform the SSL handshake
        m_ws.next_layer().async_handshake(ssl::stream_base::client, beast::bind_front_handler(&Session::on_ssl_handshake, shared_from_this()));
    }

    void on_ssl_handshake(beast::error_code ec) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "ssl_handshake");

        // Turn off the timeout on the tcp_stream, because
       // the websocket stream has its own timeout system.
        beast::get_lowest_layer(m_ws).expires_never();

        // Set suggested timeout settings for the websocket
        m_ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        m_ws.set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
                req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket_client_ssl");
        }));

        // Perform the websocket handshake
        m_ws.async_handshake(m_host, "/", beast::bind_front_handler(&Session::on_handshake, shared_from_this()));
    }

    void on_handshake(beast::error_code ec) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "handshake");

        // Send the message
        m_ws.async_write(net::buffer(m_text), beast::bind_front_handler(&Session::on_write,shared_from_this()));
    }

    void on_write(beast::error_code ec,std::size_t bytes_transferred) {
        std::cout << "Listener:" << __func__ << std::endl;
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "write");

        // Read a message into our buffer
        m_ws.async_read(m_buffer, beast::bind_front_handler(&Session::on_read, shared_from_this()));
    }

    std::string unsubscribe(std::string exchange, std::string symbol, std::string level) {
        std::ostringstream oss;
        oss << "{"
            << R"("type":"unsubscribe")"
            << R"(,"exchange":")" << exchange << R"(")"
            << R"(,"symbol":")" << symbol << R"(")"
            << R"(,"level":")" << level << R"(")"
            << "}";
	    return oss.str();
    }

    void on_read(beast::error_code ec,std::size_t bytes_transferred) {
        //std::cout << __func__ << ": bytes: " << bytes_transferred << std::endl;
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        // The make_printable() function helps print a ConstBufferSequence
        std::ostringstream oss;
        oss << beast::make_printable(m_buffer.data());
        std::cout << __func__ << "-" << ++m_msgCount << ": " << oss.str() << std::endl;
        Document doc;
        doc.Parse(oss.str().c_str());
        std::string type = doc["type"].GetString();
        if(strcmp(type.c_str(), "subscriptions") == 0) {
            // parseSubscriptions(oss.str().c_str());
        } 
        else if(strcmp(type.c_str(), "l2update") == 0) {
            parseL2updateSs(oss.str().c_str());
            // parseL2update(oss.str().c_str());
        } 
        else if(strcmp(type.c_str(), "ticker") == 0) {
            // parseTicker(oss.str().c_str());
            parseTickerSs(oss.str().c_str());
        }  
        else if(strcmp(type.c_str(), "snapshot") == 0) {
            // parseSnapshot(oss.str().c_str());
            parseSnapshotSs(oss.str().c_str());
        }

        // Clear the buffer
        m_buffer.consume(m_buffer.size());

        // Read a message into our buffer
        sleep(1);
        m_ws.async_read( m_buffer, beast::bind_front_handler(&Session::on_read, shared_from_this()));

    }

    void on_close(beast::error_code ec) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "close");

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence
        std::cout << __func__ << std::endl;
    }

private:
    tcp::resolver m_resolver;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> m_ws;
    beast::flat_buffer m_buffer;
    std::string m_host;
    std::string m_text;
    unsigned m_msgCount = 0;
};

//------------------------------------------------------------------------------

int main(int argc, char **argv) {
    if(argc < 2 || argc > 2)
    {
        std::cout << "Invalid Arguements :";
        std::cout << "\nExample:";
        std::cout << "\n\t" << argv[0] << " <Symbols>";
        std::cout << "\n\t" << argv[0] << " \"\\\"BTC-USD\\\"\"\n";
        return -1;
    }
    // Check command line arguments.
    auto const host = "ws-feed-public.sandbox.exchange.coinbase.com";
    auto const port = "443";
    std::ostringstream oss;
    oss << argv[1];
    const auto msg = fmtGdaxSubscribe(oss.str());
    std::cout << __func__ << ": " << msg << std::endl;

    // The io_context is required for all I/O
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12_client};

    // Launch the asynchronous operation
    // std::cout << text << std::endl;
    std::make_shared<Session>(ioc, ctx)->run(host, port, msg.c_str());

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();

    return EXIT_SUCCESS;
}




// Request
/*
{
    "type": "subscribe",
    "product_ids": [
        "BTC-USD"
    ],
    "channels": [
        "level2"
    ],
    "signature": "...",
    "key": "...",
    "passphrase": "...",
    "timestamp": "..."
}

*/