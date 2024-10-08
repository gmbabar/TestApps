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
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cppcodec/hex_lower.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

//------------------------------------------------------------------------------

const std::string aPubKey = "47dfc129c1135efb2348d443297aaa0c";
const std::string aPriKey = "igIjLcJHQaBCMaYpGoexS2/HQ5jdKjXX7i4qsSAJkdcjb+deOkNHf0LfD9VDg9lGM+jfK5B6zSmyEASB0PYz1A==";
const std::string aPassPhrase = "bdtvyfvpyu9";
const std::string aApiHost="api-public.sandbox.exchange.coinbase.com";
const std::string aApiPort="443";
std::vector<std::string> id;


inline uint64_t gdaxTimestamp() {
    const auto p1 = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
}

inline std::string coinbaseSignature(
        const std::string &aPriKey,
        const std::string &aPayload) {
    // DMA: Gateways/Coinbase/rest_api.cpp
    // Calculate signature
    char buffer[1024];
    auto msgSize = boost::beast::detail::base64::decode(buffer, aPriKey.c_str(), aPriKey.length()).first;
	unsigned int len = SHA256_DIGEST_LENGTH;
	unsigned char hash[SHA256_DIGEST_LENGTH];
#if OPENSSL_VERSION_NUMBER >= 0x1010100fL
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac, reinterpret_cast<const unsigned char*>(buffer), msgSize, EVP_sha256(), NULL);
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

    // Format into string
auto signature = cppcodec::hex_lower::encode(hash, len);
    // std::memset(buffer, 0, sizeof(buffer));
    msgSize = boost::beast::detail::base64::encode(buffer, hash, len);

std::cout << "Payload: " << aPayload << std::endl;
std::cout << "signature: " << signature << std::endl;
std::cout << "signature: " << std::string(buffer, msgSize) << std::endl;

    return std::string(buffer, msgSize);
}

template <typename BodyT>
void signCoinbaseRestRequest(
        boost::beast::http::request<BodyT> &aRequest) {

    // Fill common headers
    // aRequest.method(http::verb::post);   // caller knows and should set it.
    aRequest.version(11);
    aRequest.keep_alive(true);
    aRequest.set(http::field::content_type, "Application/JSON");

    // Prepare signature
    aRequest.prepare_payload();

    // Generate signature
    auto nonce = gdaxTimestamp();
    auto target = aRequest.target();

    std::stringstream ss;
    ss << nonce
       << aRequest.method()
       << target;
    if(aRequest.method() == http::verb::post)
        ss << aRequest.body();

    // Format payload
    const auto payload = ss.str();
    const auto signature = coinbaseSignature(aPriKey, payload);

    // ONLY REQUIRED FOR boost 1.7+
    ss.str("");
    ss << nonce;

    // Set auth header
    aRequest.set("CB-ACCESS-SIGN", signature);
    aRequest.set("CB-ACCESS-TIMESTAMP", ss.str());
    aRequest.set("CB-ACCESS-KEY", aPubKey);
    aRequest.set("CB-ACCESS-PASSPHRASE", aPassPhrase);

    // std::cout << "DEBUG: signCoinbaseRestRequest: Request: \n" << aRequest << std::endl;
}

inline bool fmtCoinbaseSpotRestApiOrder(
        boost::beast::http::request<boost::beast::http::string_body> &aRequest,
        std::string order) {
    namespace beast = boost::beast;
    namespace http = beast::http;

    aRequest.method(http::verb::post);
    aRequest.set(http::field::host, aApiHost);
    aRequest.target("/orders");
    aRequest.body() = order;
    signCoinbaseRestRequest(aRequest);
    return true;

}

inline bool fmtCoinbaseSpotRestGetOrder(
    boost::beast::http::request<boost::beast::http::string_body> &aRequest,
    std::string json) {

    namespace beast = boost::beast;
    namespace http = beast::http;

    aRequest.method(http::verb::post);
    aRequest.set(http::field::host, aApiHost);
    aRequest.target("/orders");
    aRequest.body() = json;
    signCoinbaseRestRequest(aRequest);
    return true;
}

inline bool fmtCoinbaseSpotRestApiCancel(
        boost::beast::http::request<boost::beast::http::string_body> &aRequest,
        std::string id) {

    namespace beast = boost::beast;
    namespace http = beast::http;

    aRequest.method(http::verb::delete_);
    aRequest.set(http::field::host, aApiHost);
    aRequest.target("/orders/"+id);
    signCoinbaseRestRequest(aRequest);
    return true;
}

inline bool fmtCoinbaseSpotRestApiProducts(
        boost::beast::http::request<boost::beast::http::string_body> &aRequest) {
    namespace beast = boost::beast;
    namespace http = beast::http;

    // Format the request
    aRequest.method(http::verb::get);
    aRequest.set(http::field::host, aApiHost);
    aRequest.target("/products");
    signCoinbaseRestRequest(aRequest);
    return true;
}

inline bool fmtCoinbaseSpotRestApiBalance(
        boost::beast::http::request<boost::beast::http::string_body> &aRequest) {
    namespace beast = boost::beast;
    namespace http = beast::http;

    // Format the request
    aRequest.method(http::verb::get);
    aRequest.set(http::field::host, aApiHost);
    aRequest.target("/coinbase-accounts");
    signCoinbaseRestRequest(aRequest);
    return true;
}

inline bool fmtGetOrders(boost::beast::http::request<boost::beast::http::string_body> &aRequest) {
    namespace beast = boost::beast;
    namespace http = beast::http;
    aRequest.method(http::verb::get);
    aRequest.set(http::field::host, aApiHost);
    aRequest.target("/orders");
    signCoinbaseRestRequest(aRequest);
    return true;
}

inline bool parseBalanceReport(std::string &json) {
    auto data = std::stringstream(json);
    boost::property_tree::ptree info;
    boost::property_tree::read_json(data, info);

    boost::property_tree::ptree::iterator it =  info.end();
    for (auto itr = info.begin(); itr != it; itr++) {
        std::cout << "Name = " << itr->second.get<std::string>("name") << std::endl
            << "Balance = " << itr->second.get<std::string>("balance") << std::endl
            << "Currency = " << itr->second.get<std::string>("currency") << std::endl
            << "Hold Balance = " << itr->second.get<std::string>("hold_balance") << std::endl
            << "Hold Currency = " << itr->second.get<std::string>("hold_currency") << std::endl
            << "-----------------------------------------------------------------" << std::endl;
    }
    return true;
}

inline std::vector<std::string> parseOpenOrders(
        boost::beast::http::request<boost::beast::http::string_body> &aRequest,
        std::string &json) {

    std::vector<std::string> orderId;
    auto data = std::stringstream(json);
    boost::property_tree::ptree info;
    boost::property_tree::read_json(data, info);

    boost::property_tree::ptree::iterator it = info.end();
    for (auto itr=info.begin(); itr != it; itr++) {
        orderId.push_back(itr->second.get<std::string>("id"));
    }

    for (auto &ordId : orderId) {
        std::cout << ordId << std::endl;
        if(std::find(id.begin(), id.end(), ordId) != id.end()) {
            std::cout << "Known Order" << std::endl;
        }
        else {
            std::cout << "Unknown Order" << std::endl;
        }
    }

    return orderId;
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
        // Format Balance Report Request
        // fmtCoinbaseSpotRestApiBalance(req_);
        id.push_back("1084bc2f-aa3c-4739-b71c-2ad265226763");
        id.push_back("f9aa164f-9639-44bd-9f30-1ab5793530a0");
        fmtGetOrders(req_);
        // Send the HTTP request to the remote host
        http::async_write(stream_, req_,
            std::bind(
                &session::on_write,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void Write() {
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
        // std::cout << data << std::endl;
        auto openOrders = parseOpenOrders(req_, data);
        // for (auto &id : openOrders) {
        //     std::ostringstream oss;
        //     oss << "{"
        //         << "\"order_id\":" << id
        //         << "}";
        //     fmtCoinbaseSpotRestGetOrder(req_, oss.str());
        //     this->Write();
        // }
        // parseBalanceReport(data);
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

    return EXIT_SUCCESS;
}
