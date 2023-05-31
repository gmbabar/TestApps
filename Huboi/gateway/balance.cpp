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
#include <boost/date_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <cppcodec/hex_lower.hpp>
#include <cppcodec/base64_rfc4648.hpp>
#include <boost/beast/core/detail/base64.hpp>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
using boost::property_tree::ptree;

//------------------------------------------------------------------------------



// inline uint64_t geminiTimestamp() {
//     // timespec now = { 0, 0 };
//     // if(clock_gettime(CLOCK_REALTIME, &now) < 0) 
//     //     return -1;
//     // std::cout << now.tv_sec + (now.tv_nsec/1000) << std::endl;
//     // return now.tv_sec/*1000000*/ + (now.tv_nsec/1000);
//     const auto p1 = std::chrono::system_clock::now().time_since_epoch();
//     return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

// }

inline std::string huobiSignature(
        const std::string &aPriKey,
        const std::string &aPayload) {
	unsigned int len = SHA256_DIGEST_LENGTH;
	unsigned char hash[SHA256_DIGEST_LENGTH];
#if OPENSSL_VERSION_NUMBER >= 0x1010100fL
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac, reinterpret_cast<const unsigned char*>(aPriKey.c_str()), aPriKey.length(), EVP_sha256(), NULL);
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
    return cppcodec::base64_rfc4648::encode(hash, len);
}


struct authInfo {
    std::string aPubKey;
    std::string aPriKey;
};

std::string ptimeToRequestTimestamp(const boost::posix_time::ptime aNow) {
    std::string timestamp = boost::posix_time::to_iso_extended_string(aNow).substr(0, 19);
    return timestamp;
}

std::string urlEncode(const std::string& input) {
    std::ostringstream encodedStream;
    encodedStream << std::hex << std::uppercase;

    for (char c : input) {
        // Include alphanumeric characters, hyphen, underscore, and period (RFC 3986)
        if (isalnum(c) || c == '-' || c == '_' || c == '.') {
            encodedStream << c;
        }
        else {
            // Convert special characters to percent-encoded representation
            encodedStream << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
        }
    }

    return encodedStream.str();
}

template <typename BodyT>
void signGeminiRestRequest(
        const std::string &aHost,
        authInfo &aAuthInfo,
        const boost::posix_time::ptime aNow,
        boost::beast::http::request<BodyT> &aRequest) {

    namespace beast = boost::beast;
    namespace http = beast::http;
    aRequest.prepare_payload();
    aRequest.version(11);
    aRequest.keep_alive(true);
    aRequest.set(http::field::content_type, "Application/JSON");

    //Generate Timestamp
    const auto requestTimestamp = ptimeToRequestTimestamp(aNow);

    if(aRequest.method() == http::verb::post) {
        std::stringstream ss;
        ss  << aRequest.method() << '\n'
            << aHost << '\n'
            << aRequest.target() << '\n'
            << "AccessKeyId=" << aAuthInfo.aPubKey
            << "&SignatureMethod=HmacSHA256"
            << "&SignatureVersion=2"
            << "&Timestamp=" << requestTimestamp;
        const auto payload = ss.str();
        const auto signature = huobiSignature(aAuthInfo.aPriKey, payload);

        //Generate New Target
        std::stringstream tss;
        tss << aRequest.target() << '?'
            << "AccessKeyId=" << aAuthInfo.aPubKey
            << "&SignatureMethod=HmacSHA256"
            << "&SignatureVersion=2"
            << "&Timestamp=" << requestTimestamp
            << "&Signature=" << urlEncode(signature);
        aRequest.target(tss.str());

        //Set header
        aRequest.set("Signature", signature);
    }
    else if(aRequest.method() == http::verb::get) {
        auto target = aRequest.target();

        //parse out args
        boost::beast::string_view args;
        auto i = target.find_first_of('?');
        if(i != std::string::npos) {
            args = target.substr(i+1);
            target = target.substr(0, i);
        }
        std::stringstream ss;
        ss  << "GET" << '\n'
            << aHost << '\n'
            << target << '\n'
            << "AccessKeyId=" << aAuthInfo.aPubKey
            << "&SignatureMethod=HmacSHA256"
            << "&SignatureVersion=2"
            << "&Timestamp=" << urlEncode(requestTimestamp);
        if(!args.empty()) {
            ss << '&' << args;
        }
        const auto payload = ss.str();

        const auto signature = huobiSignature(aAuthInfo.aPriKey, payload);

        std::stringstream tss;
        tss << aRequest.target() << '?'
            << "AccessKeyId=" << aAuthInfo.aPubKey;
        if(!args.empty()) {
            tss << '&' << args;
        }
        tss << "&SignatureMethod=HmacSHA256"
            << "&SignatureVersion=2"
            << "&Timestamp=" << requestTimestamp
            << "&Signature=" << urlEncode(signature);
        aRequest.target(tss.str());

        aRequest.set("Signature", signature);
    }

    // Set auth header
    aRequest.set(http::field::host, aHost);
    aRequest.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    aRequest.set(http::field::content_type, "application/json");
    aRequest.set(http::field::content_length, "0");
    aRequest.set(http::field::cache_control, "no-cache");

    // aRequest.set("X-GEMINI-SIGNATURE", signature);
    // aRequest.set("X-GEMINI-APIKEY", aPubKey);
    // aRequest.set("X-GEMINI-PAYLOAD", payload);
}


//------------------------------------------------------------------------------

// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

void ParseAccounts(const std::string& jsonString, std::vector<int> &id) {
    boost::property_tree::ptree pt;
    std::istringstream iss(jsonString);

    boost::property_tree::read_json(iss, pt);

    boost::property_tree::ptree data = pt.get_child("data");
    for (const auto& account : data) {
        std::string state = account.second.get<std::string>("state");
        if(state == "working") {
            id.push_back(account.second.get<int>("id"));
        }
    }
}

void ParseBalances(const std::string& jsonString)
{
    ptree pt;

    try {
        std::istringstream iss(jsonString);
        read_json(iss, pt);
    } catch (const boost::property_tree::json_parser::json_parser_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }

    std::string status = pt.get<std::string>("status");
    std::cout << "Status: " << status << std::endl;

    ptree data = pt.get_child("data");
    int id = data.get<int>("id");
    std::string type = data.get<std::string>("type");
    std::string state = data.get<std::string>("state");

    std::cout << "Data - ID: " << id << ", Type: " << type << ", State: " << state << std::endl;

    ptree listArray = data.get_child("list");
    for (const auto& item : listArray) {
        std::string currency = item.second.get<std::string>("currency");
        std::string balance = item.second.get<std::string>("balance");
        std::string debt = item.second.get<std::string>("debt");
        std::string seqNum = item.second.get<std::string>("seq-num");

        std::cout << "List Array - Currency: " << currency << ", Balance: " << balance
                  << ", Debt: " << debt << ", Seq-Num: " << seqNum << std::endl;
    }
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
    // std::string m_orderId = "2224168876";
    // Resolver and stream require an io_context
    explicit
    session(boost::asio::io_context& ioc, ssl::context& ctx)
        : resolver_(ioc)
        , stream_(ioc, ctx)
    {
        // const std::string aPubKey = "7d393344-1qdmpe4rty-37def1b5-d10e3";
// const std::string aPriKey = "84aadfc7-a69c48d2-e5538ffc-e0f8c";
        m_authInfo.aPubKey = "7d393344-1qdmpe4rty-37def1b5-d10e3";
        m_authInfo.aPriKey = "84aadfc7-a69c48d2-e5538ffc-e0f8c";
    }

    // Start the asynchronous operation
    void
    run()
    {
        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(stream_.native_handle(), m_host.c_str()))
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
            m_host,
            m_port,
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

// const std::string aPubKey = "7d393344-1qdmpe4rty-37def1b5-d10e3";
// const std::string aPriKey = "84aadfc7-a69c48d2-e5538ffc-e0f8c";
// const std::string aApiHost="api.huobi.pro";
// // api.sandbox.gemini.com/v1
// const std::string aApiPort="443";
        // req_.target("/v1/account/accounts/56649264/balance");
        req_.target("/v1/account/accounts");
        // fmtGeminiSpotRestApiCancel(req_, m_orderId);
        boost::posix_time::ptime currentTime = boost::posix_time::second_clock::universal_time();
        signGeminiRestRequest(m_host, m_authInfo, currentTime, req_);
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
        res_.clear();
        if(not accountsReceived) {
            ParseAccounts(data, m_ids);
            std::stringstream ss;
            for (const auto &id : m_ids) {
                ss  << "/v1/account/accounts/" << id <<"/balance";
                std::cout << ss.str() << std::endl;
                req_.target(ss.str());
                boost::posix_time::ptime currentTime = boost::posix_time::second_clock::universal_time();
                signGeminiRestRequest(m_host, m_authInfo, currentTime, req_);
                http::async_write(stream_, req_,
                    std::bind(
                        &session::on_write,
                        shared_from_this(),
                        std::placeholders::_1,
                        std::placeholders::_2));
            }
            accountsReceived = true;
        }
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
private:
    std::string m_host = "api.huobi.pro";
    std::string m_port = "443";
    authInfo m_authInfo;
    std::vector<int> m_ids;
    bool accountsReceived = false;
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
    ctx.set_verify_mode(ssl::verify_peer);
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
