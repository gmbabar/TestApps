//------------------------------------------------------------------------------
//
// Example: HTTP SSL client, asynchronous
// - https://www.boost.org/doc/libs/1_69_0/libs/beast/example/http/client/async-ssl/http_client_async_ssl.cpp
//
//------------------------------------------------------------------------------
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <thread>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <cppcodec/hex_lower.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace rapidjson;

//------------------------------------------------------------------------------

inline uint64_t OkexTimestamp() {
    const auto p1 = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
}

inline std::string OkexSignature(
        const std::string &aPriKey,
        const std::string &aPayload) {
    // DMA: Gateways/Coinbase/rest_api.cpp
	unsigned int len = SHA256_DIGEST_LENGTH;
	unsigned char hash[SHA256_DIGEST_LENGTH];
#if OPENSSL_VERSION_NUMBER >= 0x1010100fL
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac, reinterpret_cast<const unsigned char*>(aPriKey.c_str()), aPriKey.size(), EVP_sha256(), NULL);
    HMAC_Update(hmac, reinterpret_cast<const unsigned char*>(aPayload.c_str()), aPayload.length());
    HMAC_Final(hmac, hash, &len);
    HMAC_CTX_free(hmac);
#else
    HMAC_CTX hmac;
    HMAC_CTX_init(&hmac);
    HMAC_Init_ex(&hmac, reinterpret_cast<const unsigned char*>(buffer), size, EVP_sha256(), NULL);
    HMAC_Update(&hmac, reinterpret_cast<const unsigned char*>(aPayload.c_str()), aPayload.length());
    HMAC_Final(&hmac, hash, &len);
    HMAC_CTX_cleanup(&hmac);
#endif

    // Encode Into Base64
    char buffer[1024];
    const auto msgSize = boost::beast::detail::base64::encode(buffer, hash, len);

    return std::string(buffer, msgSize);
}

struct OkexAuthInfo {
    std::string aPubKey;
    std::string aPriKey;
    std::string aPassPhrase;
};

inline std::string fmtOkexLogin(
        const OkexAuthInfo &aAuthInfo) {

    const auto timestamp = OkexTimestamp();
    std::stringstream ss;
    ss  << timestamp 
        << "GET"
        << "/users/self/verify";
    const auto payload = ss.str();
    const auto signature = OkexSignature(aAuthInfo.aPriKey, payload);

    std::stringstream qss;
    qss << "{\"op\":\"login\", \"args\": ["
        << "{\"apiKey\": \"" << aAuthInfo.aPubKey << '"'
        << ",\"passphrase\": \"" << aAuthInfo.aPassPhrase << '"'
        << ",\"timestamp\":\"" << timestamp << '"'
        << ",\"sign\":\"" << signature << '"'
        << "}]}";
    std::cout << qss.str() << std::endl;
    return qss.str();

}

inline std::string fmtOkexWebsocketSubscribeChannel(
        const std::string &aChannel) {

    std::stringstream ss;
    ss  << "{\"op\":\"subscribe\", \"args\": ["
        << "{\"channel\": \"" << aChannel << "\"}]}";

    return ss.str();
}

inline std::string fmtOkexWebsocketSubscribeChannel(
        const std::string &aChannel,
        const std::string &anExchSym) {

    std::stringstream ss;
    ss  << "{\"op\":\"subscribe\", \"args\": ["
        << "{\"channel\": \"" << aChannel << "\""
        << ",\"instType\":\"ANY\""
        << ",\"instId\":\"" << anExchSym << "\"}]}";
    return ss.str();
}

// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

class Session : public std::enable_shared_from_this<Session> {


public:
    // Resolver and socket require an io_context
    explicit Session(net::io_context& ioc, ssl::context& ctx)
        : m_resolver(net::make_strand(ioc))
        , m_ws(net::make_strand(ioc), ctx) {
    }

    // Start the asynchronous operation
    void run( char const* host, char const* port, OkexAuthInfo &authInfo) {
        std::cout << "Listener:" << __func__ << std::endl;
        // Save these for later
        m_host = host;
        m_authInfo = authInfo;

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
                req.set("x-simulated-trading", "1");
        }));

        // Perform the websocket handshake
        m_ws.async_handshake(m_host, "/ws/v5/private?brokerId=9999", beast::bind_front_handler(&Session::on_handshake, shared_from_this()));
    }

    void on_handshake(beast::error_code ec) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "handshake");

        // Send the message

        std::string msg = fmtOkexLogin(m_authInfo);
        m_ws.async_write(net::buffer(msg), beast::bind_front_handler(&Session::on_write,shared_from_this()));
    }

    void on_write(beast::error_code ec,std::size_t bytes_transferred) {
        std::cout << "Listener:" << __func__ << std::endl;
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "write");

        // Read a message into our buffer
        m_ws.async_read(m_buffer, beast::bind_front_handler(&Session::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec,std::size_t bytes_transferred) {
        //std::cout << __func__ << ": bytes: " << bytes_transferred << std::endl;
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        // The make_printable() function helps print a ConstBufferSequence
        std::ostringstream oss;
        oss << beast::make_printable(m_buffer.data());
        m_buffer.consume(m_buffer.max_size());
        std::cout << __func__ << "-" << m_msgCount + 1 << ": " << oss.str() << std::endl;
        Document doc;
        doc.Parse(oss.str().c_str());
        const auto event = (doc.HasMember("event") && doc["event"].IsString() ? doc["event"].GetString():"");
        // const auto event = "";
        ++m_msgCount;
        if (!m_subscribed) {
            std::string msg = fmtOkexWebsocketSubscribeChannel("balance_and_position");
            m_subscribed = true;
            m_ws.async_write(net::buffer(msg), beast::bind_front_handler(&Session::on_write,shared_from_this()));
        }
        else {
            m_ws.async_read(m_buffer, beast::bind_front_handler(&Session::on_read, shared_from_this()));
        }

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
    OkexAuthInfo m_authInfo;
    unsigned m_msgCount = 0;
    bool m_subscribed = false;
};

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    // The io_context is required for all I/O
    boost::asio::io_context ioc;

    OkexAuthInfo authInfo;
    authInfo.aPubKey = "415c3127-6b61-478f-bd21-94c946bd7a62";
    authInfo.aPriKey = "91878CFAFE56860490C2F977FDCEBAF7";
    authInfo.aPassPhrase = "ForTestingOnTST.1";

    // fmtOkexLogin(authInfo);

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::sslv23_client};

    // This holds the root certificate used for verification
    // load_root_certificates(ctx);
    const char *host = "wspap.okx.com";
    const char *port = "8443";
    
    // Verify the remote server's certificate
    ctx.set_verify_mode(ssl::verify_peer);
    ctx.set_verify_mode(boost::asio::ssl::verify_none);

    // Launch the asynchronous operation
    std::make_shared<Session>(ioc, ctx)->run(host, port, authInfo);
    // std::make_shared<session>(ioc, ctx)->run();

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return EXIT_SUCCESS;
}
