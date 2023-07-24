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
#include <iomanip>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cppcodec/hex_lower.hpp>
#include <cppcodec/hex_upper.hpp>
#include <cppcodec/base64_rfc4648.hpp>
#include <boost/beast/core/detail/base64.hpp>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using base64 = cppcodec::base64_rfc4648;
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

//------------------------------------------------------------------------------

// const std::string aPubKey = "Liut6ciYIYn4QRGT6rT9xomWSNU2d25z4LSMwatGbyoB4eBmPbGNQ/Iu ";
// const std::string aPriKey = "Kfx1vpSiT9VzmHzvJZAz1H6NZwET5HLQdaeIjJR3Dfe2zpBjGhMee0dhYFeuXwu2ZtKyC48L0FY05G8a8k9omw==";
const auto aPubKey = "pZW/cAyUQfpPnubzo1v3Vj6KlDT7QklrmNCJiPDlRNxZLvRqjDuWawod";
const auto aPriKey = "jQSIs/VFwIME1BD74THcV5irnTG0/nzPNgBK6lfCebhqHMqONSlGARWqm1wLrOjYqFWW86/eV01jNv5cIQhM5g==";
const std::string aApiHost="api.kraken.com";
const std::string aApiPort="443";

void create_sha256(const std::string& data, unsigned char *buffer) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(buffer, &sha256);
}

inline std::string krakenSignature(const std::string &aPriKey, const std::string &aPayload) {
    unsigned int len = SHA512_DIGEST_LENGTH;
    unsigned char hash[SHA512_DIGEST_LENGTH];
    char buffer[1024];
    auto msgSize = boost::beast::detail::base64::decode(buffer, aPriKey.c_str(), aPriKey.size()).first;
#if OPENSSL_VERSION_NUMBER >= 0x1010100fL
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac, reinterpret_cast<const unsigned char*>(buffer), msgSize, EVP_sha512(), NULL);
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
    //return cppcodec::hex_lower::encode(hash, len);
    msgSize = boost::beast::detail::base64::encode(buffer, hash, len);
    return std::string(buffer, msgSize);
}


std::string get_kraken_signature(const std::string& urlpath, const std::string &nonce, const std::string& data, const std::string& secret) {

    // construct payload
    char buffer[SHA256_DIGEST_LENGTH];
    std::string rawMsg = nonce + data;
    create_sha256(rawMsg, (unsigned char*)buffer);
    std::string mesg = urlpath + std::string(buffer, SHA256_DIGEST_LENGTH);

    return krakenSignature(aPriKey, mesg);
}


template <typename BodyT>
void signKrakenRestRequest(
        boost::beast::http::request<BodyT> &aRequest, const std::string &nonce, const std::string &data) {

    aRequest.body() = data;

    // Fill common headers
    aRequest.method(http::verb::post);   // caller knows and should set it.
    aRequest.version(11);
    aRequest.keep_alive(true);
    //aRequest.set(http::field::content_type, "Application/JSON");

    // Generate signature
    const auto signature = get_kraken_signature(aRequest.target().data(), nonce, data, aPriKey);

    // Set auth header
    aRequest.set(http::field::host, aApiHost);
    aRequest.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    aRequest.set(http::field::content_type, "application/json");
    aRequest.set(http::field::cache_control, "no-cache");

    aRequest.set("API-Sign", signature);
    aRequest.set("API-Key", aPubKey);

    // Prepare final request
    aRequest.prepare_payload();

    std::cout << "Request: \n" << aRequest << std::endl;
}


template <typename BodyT>
inline bool fmtKrakenSpotRestApiNewOrder(
    boost::beast::http::request<BodyT> &aRequest) {

    const auto target = "/0/private/AddOrder";
    const auto nonce = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch()).count());
    const auto &type = "limit";
    const auto &side = "buy";
    const auto &sym = "XBTUSD";
    const auto qty = 0.0001;
    const auto px  = 17500.0;

    aRequest.target(target);
    std::ostringstream oss;
    oss << "{"
        << "\"nonce\":" << nonce
        << ",\"ordertype\":\"" << type << "\""
        << ",\"type\":\"" << side << "\""
        << ",\"volume\":" << qty
        << ",\"pair\":\"" << sym << "\""
        << ",\"price\":" << px
        << "}";

    signKrakenRestRequest(aRequest, nonce, oss.str());
    return true;
}



//------------------------------------------------------------------------------

// Report a failure
void fail(boost::system::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

// Performs an HTTP GET and prints the response
class RestSession : public std::enable_shared_from_this<RestSession> {
    tcp::resolver resolver_;
    ssl::stream<tcp::socket> stream_;
    boost::beast::flat_buffer buffer_; // (Must persist between reads)
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
    bool orderCreated = false;

public:
    // Resolver and stream require an io_context
    explicit
    RestSession(boost::asio::io_context& ioc, ssl::context& ctx)
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
                &RestSession::on_resolve,
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
                &RestSession::on_connect,
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
                &RestSession::on_handshake,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    on_handshake(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "handshake");


        // fmtGeminiSpotRestApiOrderStatus(req_, "2294408759", true);
        fmtKrakenSpotRestApiNewOrder(req_);
        // Send the HTTP request to the remote host
        http::async_write(stream_, req_,
            std::bind(
                &RestSession::on_write,
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
                &RestSession::on_read,
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
        sleep(1);
        auto data = res_.body();
        std::cout << "Body: " << data << std::endl;
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

// ------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    // The io_context is required for all I/O
    boost::asio::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::sslv23_client};

    // Verify the remote server's certificate
    // ctx.set_verify_mode(ssl::verify_peer);
    ctx.set_verify_mode(boost::asio::ssl::verify_none);

    // Launch the asynchronous operation
    std::make_shared<RestSession>(ioc, ctx)->run();

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return EXIT_SUCCESS;
}
