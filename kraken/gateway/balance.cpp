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
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <cppcodec/hex_lower.hpp>
#include <cppcodec/hex_upper.hpp>
#include <cppcodec/base64_rfc4648.hpp>
#include <boost/beast/core/detail/base64.hpp>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
// using sha256 = cppcodec::hex_lower<cppcodec::sha256>;
using base64 = cppcodec::base64_rfc4648;
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

//------------------------------------------------------------------------------

const std::string aPubKey = "Liut6ciYIYn4QRGT6rT9xomWSNU2d25z4LSMwatGbyoB4eBmPbGNQ/Iu ";
const std::string aPriKey = "Kfx1vpSiT9VzmHzvJZAz1H6NZwET5HLQdaeIjJR3Dfe2zpBjGhMee0dhYFeuXwu2ZtKyC48L0FY05G8a8k9omw==";
const std::string aApiHost="api.kraken.com";
const std::string aApiPort="443";



std::string decodeBase(std::string &msg) {
    char buffer[1024];
    auto msgSize = boost::beast::detail::base64::decode(buffer, msg.c_str(), msg.length()).first;

}

inline std::string krakenSignature(
        const std::string &aPriKey,
        const std::string &aPayload) {

    char buffer[1024];
    auto msgSize = boost::beast::detail::base64::decode(buffer, aPriKey.c_str(), aPriKey.length()).first;
    std::cout << buffer << std::endl;
	unsigned int len = SHA512_DIGEST_LENGTH;
	unsigned char hash[SHA512_DIGEST_LENGTH];
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
    // auto signature = cppcodec::hex_lower::encode(hash, len);
    msgSize = boost::beast::detail::base64::encode(buffer, hash, len);
    return std::string(buffer, msgSize);
    // return signature;
}


std::string sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}


std::string get_kraken_signature(const std::string& urlpath, const std::string& data, const std::string& secret) {
    std::ostringstream oss;
    oss << "{\"nonce\": " << data << "}";

    std::string encoded = data + oss.str();
    std::cout << encoded << std::endl;

    std::string sha256hash = sha256(encoded);
    std::cout << sha256hash << std::endl;
    std::string message = urlpath + sha256hash;
    std::cout << message << std::endl;

    HMAC_CTX* ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, base64::decode(secret).data(), base64::decode(secret).size(), EVP_sha512(), NULL);
    HMAC_Update(ctx, reinterpret_cast<const unsigned char*>(message.c_str()), message.size());

    unsigned char mac[EVP_MAX_MD_SIZE];
    unsigned int mac_len;
    HMAC_Final(ctx, mac, &mac_len);
    HMAC_CTX_free(ctx);

    // std::cout << std::string(mac, mac_len) << std::endl;
    std::cout << cppcodec::hex_lower::encode(mac, mac_len) << std::endl;
    std::string sigdigest = base64::encode(mac, mac_len);
    std::cout << sigdigest << std::endl;
    return sigdigest;
    // return sha256hash;
}


template <typename BodyT>
void signKrakenRestRequest(
        boost::beast::http::request<BodyT> &aRequest, const char *data) {

    // Fill common headers
    aRequest.method(http::verb::post);   // caller knows and should set it.
    aRequest.version(10);
    aRequest.keep_alive(true);
    aRequest.set(http::field::content_type, "Application/JSON");

    // Prepare signature
    aRequest.prepare_payload();

    // Generate signature
    // auto nonce = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    const auto signature = get_kraken_signature(aRequest.target(), data, aPriKey);
    // std::ostringstream oss;
    // oss << nonce << data;
    // const auto postData = oss.str();
    // std::cout << postData << std::endl;
    // auto signature = krakenRequestSignature(postData);
    // std::cout << signature << std::endl;

    // oss.str("");
    // oss << aRequest.target() << signature;
    // const auto payload = oss.str();

    // // auto target = aRequest.target();
    // signature = krakenSignature(aPriKey, payload);
    // std::cout << signature << std::endl;

    // ONLY REQUIRED FOR boost 1.7+
    // ss.str("");
    // ss << nonce;

    // Set auth header
    aRequest.set(http::field::host, aApiHost);
    aRequest.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    aRequest.set(http::field::content_type, "application/json");
    aRequest.set(http::field::content_length, "0");
    aRequest.set(http::field::cache_control, "no-cache");

    aRequest.set("API-Sign", signature);
    aRequest.set("API-Key", aPubKey);
}


template <typename BodyT>
inline bool fmtKrakenSpotRestApiBalance(
    boost::beast::http::request<BodyT> &aRequest) {

    std::ostringstream oss;
    aRequest.target("/0/private/Balance");
    oss << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    signKrakenRestRequest(aRequest, oss.str().c_str());
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


        // fmtGeminiSpotRestApiOrderStatus(req_, "2294408759", true);
        fmtKrakenSpotRestApiBalance(req_);
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
        sleep(1);
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

// ------------------------------------------------------------------------------

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