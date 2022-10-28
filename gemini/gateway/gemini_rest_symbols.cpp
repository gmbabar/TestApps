//------------------------------------------------------------------------------
//
// Example: HTTP SSL client, asynchronous
// - https://www.boost.org/doc/libs/1_69_0/libs/beast/example/http/client/async-ssl/http_client_async_ssl.cpp
//
//------------------------------------------------------------------------------


#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <cppcodec/hex_lower.hpp>
#include <boost/beast/core/detail/base64.hpp>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

//------------------------------------------------------------------------------

const std::string aPubKey = "account-8BDIOhP2VnBAzQHAn6DT";
const std::string aPriKey = "mXob3f85YcHu2KEQDxXeVSMtabL";
const std::string aApiHost="api.sandbox.gemini.com";
const std::string aApiPort="443";


// inline uint64_t geminiTimestamp() {
//     // timespec now = { 0, 0 };
//     // if(clock_gettime(CLOCK_REALTIME, &now) < 0) 
//     //     return -1;
//     // std::cout << now.tv_sec + (now.tv_nsec/1000) << std::endl;
//     // return now.tv_sec/*1000000*/ + (now.tv_nsec/1000);
//     const auto p1 = std::chrono::system_clock::now().time_since_epoch();
//     return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

// }

inline std::string geminiSignature(
        const std::string &aPriKey,
        const std::string &aPayload) {
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
    HMAC_Init_ex(&hmac, reinterpret_cast<const unsigned char*>(aPriKey.c_str()), aPriKey.length(), EVP_sha256(), NULL);
    HMAC_Update(&hmac, reinterpret_cast<const unsigned char*>(aPayload.c_str()), aPayload.length());
    HMAC_Final(&hmac, hash, &len);
    HMAC_CTX_cleanup(&hmac);
#endif

    // Format into string
    auto signature = cppcodec::hex_lower::encode(hash, len);
    return signature;
}


template <typename BodyT>
void signGeminiRestRequest(
        boost::beast::http::request<BodyT> &aRequest, const char *payload) {

    // Fill common headers
    aRequest.method(http::verb::post);   // caller knows and should set it.
    aRequest.version(10);
    aRequest.keep_alive(true);
    aRequest.set(http::field::content_type, "Application/JSON");

    // Prepare signature
    aRequest.prepare_payload();

    // Generate signature
    auto nonce = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto target = aRequest.target();

    std::stringstream ss;
    const auto signature = geminiSignature(aPriKey, payload);

    // ONLY REQUIRED FOR boost 1.7+
    ss.str("");
    ss << nonce;

    // Set auth header
    aRequest.set(http::field::content_type, "application/json");
    aRequest.set(http::field::content_length, "0");
    aRequest.set(http::field::cache_control, "no-cache");

    aRequest.set("X-GEMINI-SIGNATURE", signature);
    aRequest.set("X-GEMINI-APIKEY", aPubKey);
    aRequest.set("X-GEMINI-PAYLOAD", payload);
}
template <typename BodyT>
inline bool fmtGeminiSpotRestApiOrder(
        boost::beast::http::request<BodyT> &aRequest,
        std::string symbol, std::string amount, std::string price,
        std::string side, std::string type, std::string options) {
        std::ostringstream oss;
        aRequest.target("/v1/order/new");
        oss << "{"
            << R"("request": "/v1/order/new")"
            << R"(, "nonce": )" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
            << R"(, "symbol": ")" << symbol << '"'
            << R"(, "amount": ")" << amount << '"'
            << R"(, "price": ")" << price << '"'
            << R"(, "side": ")" << side << '"'
            << R"(, "type": ")" << type << '"'
            << R"(, "options": [")" << options << "\"]"
            << "}";
        auto sPayload = oss.str();
        char payload[1024];
        auto size = boost::beast::detail::base64::encode(payload, sPayload.c_str(), sPayload.length());
        payload[size] = '\0';
        signGeminiRestRequest(aRequest, payload);
        return true;
}

//------------------------------------------------------------------------------

// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Performs an HTTP GET and prints the response
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    ssl::stream<tcp::socket> stream_;
    boost::beast::flat_buffer buffer_; // (Must persist between reads)
    // http::request<http::empty_body> req_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
    bool orderCreated = false;

public:
    // Resolver and stream require an io_context
    explicit
    session(boost::asio::io_context& ioc, ssl::context& ctx)
        : resolver_(ioc)
        , stream_(ioc, ctx)
    {
    }

    // Start the asynchronous operation
    void
    run()
    {
        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(stream_.native_handle(), aApiHost.c_str()))
        {
            boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            std::cerr << __func__ << ": " << ec.message() << "\n";
            return;
        }

        // Set up an HTTP GET request message
        // req_.version(version);
        req_.method(http::verb::get);
        req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Look up the domain name
        resolver_.async_resolve(
            aApiHost,
            aApiPort,
            std::bind(
                &session::on_resolve,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    on_resolve(
        boost::system::error_code ec,
        tcp::resolver::results_type results)
    {
        if(ec)
            return fail(ec, "resolve");

        // Make the connection on the IP address we get from a lookup
        boost::asio::async_connect(
            stream_.next_layer(),
            results.begin(),
            results.end(),
            std::bind(
                &session::on_connect,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    on_connect(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "connect");

        // Perform the SSL handshake
        stream_.async_handshake(
            ssl::stream_base::client,
            std::bind(
                &session::on_handshake,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    on_handshake(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "handshake");

        // fmtGeminiSpotRestApiOrder(req_, "btcusd", "1", "9459.15", "buy", "exchange limit", "maker-or-cancel");
        // std::cout << req_.target() << std::endl;
        // std::cout << aApiHost << std::endl;
        // std::cout << req_.method() << std::endl;
        req_.method(http::verb::get);
        req_.target("/symbols");
        // Send the HTTP request to the remote host
        http::async_write(stream_, req_,
            std::bind(
                &session::on_write,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    on_write(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "write");

        // Receive the HTTP response
        http::async_read(stream_, buffer_, res_,
            std::bind(
                &session::on_read,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    on_read(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        // Write the message to standard out
        auto data = res_.body();
        std::cout << "Body: " << data << std::endl << std::endl << std::endl;
    }

    void
    on_shutdown(boost::system::error_code ec)
    {
        if(ec == boost::asio::error::eof)
        {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec.assign(0, ec.category());
        }
        if(ec)
            return fail(ec, "shutdown");

        // If we get here then the connection is closed gracefully
    }
};

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    // The io_context is required for all I/O
    boost::asio::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::sslv23_client};

    // This holds the root certificate used for verification
    // load_root_certificates(ctx);
    // Verify the remote server's certificate
    // ctx.set_verify_mode(ssl::verify_peer);
    ctx.set_verify_mode(boost::asio::ssl::verify_none);

    // Launch the asynchronous operation
    // std::make_shared<session>(ioc, ctx)->run(host, port, target, version);
    std::make_shared<session>(ioc, ctx)->run();

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    // boost::beast::http::request<std::string> req_;
    // fmtGeminiSpotRestApiOrder(req_, "btcusd", "1", "9459.15", "buy", "exchange limit", "maker-or-cancel");

    return EXIT_SUCCESS;
}
