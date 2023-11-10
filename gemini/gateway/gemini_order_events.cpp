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
#include <cppcodec/base32_default_crockford.hpp>
#include <cppcodec/base64_default_url.hpp>
#include <cppcodec/base64_rfc4648.hpp>
#include <cppcodec/hex_lower.hpp>
#include "../include/parser.hpp"


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


// Gateways/Gemini/rest_api.cpp
inline std::string geminiSignature(
        const std::string &aPriKey,
        const std::string &aPayload) {
    // DMA: Gateways/Gemini/rest_api.cpp
    // Calculate signature
	unsigned int len = SHA384_DIGEST_LENGTH;
	unsigned char hash[SHA384_DIGEST_LENGTH];
#if OPENSSL_VERSION_NUMBER >= 0x1010100fL
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac, reinterpret_cast<const unsigned char*>(aPriKey.c_str()), aPriKey.length(), EVP_sha384(), NULL);
    HMAC_Update(hmac, reinterpret_cast<const unsigned char*>(aPayload.c_str()), aPayload.length());
    HMAC_Final(hmac, hash, &len);
    HMAC_CTX_free(hmac);
#else
    HMAC_CTX hmac;
    HMAC_CTX_init(&hmac);
    HMAC_Init_ex(&hmac, reinterpret_cast<const unsigned char*>(aPriKey.c_str()), aPriKey.length(), EVP_sha384(), NULL);
    HMAC_Update(&hmac, reinterpret_cast<const unsigned char*>(aPayload.c_str()), aPayload.length());
    HMAC_Final(&hmac, hash, &len);
    HMAC_CTX_cleanup(&hmac);
#endif

    // Format into string
    return cppcodec::hex_lower::encode(hash, len);
}


void fail(beast::error_code ec, char const* what) {
    std::cout << "Error: " << what << ":" << ec.message() << std::endl; 
}

class Session : public std::enable_shared_from_this<Session> {

public:
    explicit Session(net::io_context& ioc, ssl::context& ctx)
        :m_resolver(net::make_strand(ioc)),
        m_ws(net::make_strand(ioc), ctx) {
    }

    
    void run( char const* host, char const* port, char const* target, const std::string &pubKey, const std::string &priKey) {
    // void run( char const* host, char const* port) {
        std::cout << "Listener:" << __func__ << std::endl;
        m_host = host;
        m_target = target;
        m_pubKey = pubKey;
        m_priKey = priKey;
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

        // TESTING - disable port appending to m_host to match handshake with python example
        // m_host += ':' + std::to_string(ep.port());

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


        // Send the message
        std::ostringstream oss;
        oss << "{"
            << R"("request": ")" << m_target << '"'
            << R"(, "nonce": )" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
            << "}";

        std::string payload = oss.str();
        std::ostringstream msg;
        char encoded_payload[1024];
        auto encSize = boost::beast::detail::base64::encode(encoded_payload, payload.c_str(), payload.size());
        encoded_payload[encSize] = '\0';
        const auto signature = geminiSignature(m_priKey, encoded_payload);


        // Set a decorator to change the User-Agent of the handshake
        m_ws.set_option(websocket::stream_base::decorator([&](websocket::request_type& req) {
                req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket_client_ssl");
                req.set("X-GEMINI-PAYLOAD", encoded_payload);
                req.set("X-GEMINI-APIKEY", m_pubKey);
                req.set("X-GEMINI-SIGNATURE", signature);
                std::cout << "Listener:" << __func__ << "Payload: " << payload << std::endl;
                std::cout << "Listener:" << __func__ << "REQ: \n" << req << std::endl;
        }));

        // // Set a decorator to change the User-Agent of the handshake
        // m_ws.set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
        // }));

        // Perform the websocket handshake
        m_ws.async_handshake(m_host, m_target, beast::bind_front_handler(&Session::on_handshake, shared_from_this()));
    }

    void on_handshake(beast::error_code ec) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "handshake");

        // // Send the message
        // std::ostringstream oss;
        // oss << "{"
        //     << R"("request":")" << m_target << '"'
        //     << R"(, "nonce":)" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
        //     << "}";
        // std::string payload = oss.str();
        // std::ostringstream msg;
        // char encoded_payload[1024];
        // auto encSize = boost::beast::detail::base64::encode(encoded_payload, payload.c_str(), payload.size());
        // encoded_payload[encSize] = '\0';
        // const auto signature = geminiSignature(m_priKey, encoded_payload);

        // // Set a decorator to change the User-Agent of the handshake
        // m_ws.set_option(websocket::stream_base::decorator([&](websocket::request_type& req) {
        //         req.set("X-GEMINI-PAYLOAD", encoded_payload);
        //         req.set("X-GEMINI-APIKEY", m_pubKey);
        //         req.set("X-GEMINI-SIGNATURE", signature);
        // }));

        // m_ws.async_write(net::buffer(oss.str()), beast::bind_front_handler(&Session::on_write,shared_from_this()));

        // Read a message into our buffer
        m_ws.async_read(m_buffer, beast::bind_front_handler(&Session::on_read, shared_from_this()));
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
        // std::cout << __func__ << ": bytes: " << bytes_transferred << std::endl;

        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        // The make_printable() function helps print a ConstBufferSequence
        std::ostringstream oss;
        oss << beast::make_printable(m_buffer.data());
        std::cout << __func__ << "-" << ": " << oss.str() << std::endl;
        // Document doc;
        // doc.Parse(oss.str().c_str());

        // Clear the buffer
        m_buffer.consume(m_buffer.size());
        m_ws.async_read(m_buffer, beast::bind_front_handler(&Session::on_read, shared_from_this()));
        // Clear the buffer
        // m_buffer.consume(m_buffer.size());
    }
    void on_close(beast::error_code ec) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "close");
        std::cout << __func__ << std::endl;
    }
private:
    tcp::resolver m_resolver;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> m_ws;
    beast::flat_buffer m_buffer;
    std::string m_host;
    std::string m_target;
    std::string m_pubKey;
    std::string m_priKey;
    int count = 0;
};



int main(int argc, char **argv) {
    // Check command line arguments.

    auto const port = "443";
    auto const target = "/v1/order/events";

    // SANDBOX:
    auto const host = "api.sandbox.gemini.com";
    const std::string pubKey = "account-8BDIOhP2VnBAzQHAn6DT";
    const std::string priKey = "mXob3f85YcHu2KEQDxXeVSMtabL";

    // // PRODUCTION:
    // auto const host = "api.gemini.com";
    // const std::string pubKey = "account-lcDNZUZCQia4UgBOJFgD";
    // const std::string priKey = "3Vij4y712WfhJcf3SNZHyJqaZZgD";

    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12_client};

    std::make_shared<Session>(ioc, ctx)->run(host, port, target, pubKey, priKey);
    // std::make_shared<Session>(ioc, ctx)->run(host, port);

    ioc.run();

    return EXIT_SUCCESS;
}