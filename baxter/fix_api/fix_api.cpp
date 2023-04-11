#include <boost/asio.hpp>
#include <iostream>
#include <boost/asio/ssl.hpp>
#include <cppcodec/hex_lower.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include "load_cert.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <boost/bind/bind.hpp>

using boost::asio::ip::tcp;

typedef ssl::stream<tcp::socket> sslSocket;

const std::string aApiHost="10.208.37.13";
const std::string aApiPort="8888";
// const std::string aPubKey = "c636bc5e6a36f1089f90e1c05ccc4d18";
// const std::string aPriKey = "a1e3BCpqDv7V7MfFaFbuWzB3DJtQBjhgRHEe0WTP9D5TdRoKEo5TIRywA1YKS4eGnarjcNCUWs1oneGBasxY+g==";
// const std::string aPassPhrase = "ncz86pr00bh";
const char delim = '\x01';

inline uint64_t gdaxTimestamp() {
    const auto p1 = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
}

inline std::string fmtSendTime() {
    const auto &tp = std::chrono::system_clock::now();
    const std::string &format = "%Y%m%d-%H:%M:%S";
    std::chrono::system_clock::time_point::duration tt = tp.time_since_epoch();
    const time_t durS = std::chrono::duration_cast<std::chrono::seconds>(tt).count();
    std::ostringstream ss;
    if (const std::tm *tm = std::gmtime(&durS)) {
        ss << std::put_time(tm, format.c_str());
        // Add milliseconds.
        const long long durMs = std::chrono::duration_cast<std::chrono::milliseconds>(tt).count();
        ss << '.' << std::setw(3) << std::setfill('0') << int(durMs - durS * 1000);
    }
    else {
        ss << "<FORMAT ERROR>";
    }
    return ss.str();
}

inline std::string calculateSignature(
        const std::string &aPriKey,
        const std::string &aPayload) {
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

std::cout << "Payload: ";
for ( const auto &ch : aPayload)
    std::cout << (int(ch) == 1 ? '|' : ch);
std::cout << std::endl;
// std::cout << "signature: " << signature << std::endl;
// std::cout << "signature: " << std::string(buffer, msgSize) << std::endl;

    return std::string(buffer, msgSize);
}

std::string orderStatus(){
    std::ostringstream oss;
    oss << "37=12381203" << delim << "11=3123123" << delim << "55=btc-usd";
    return oss.str();
}

unsigned int generateCheckSum(const char *buf, long bufLen) {
    unsigned int cks = 0;
    for(long idx = 0L; idx < bufLen; idx++)
            cks += (unsigned int)buf[idx];
    return cks % 256;
}


std::string loginRequest() {
    std::ostringstream pre;
    std::ostringstream hdr;
    std::ostringstream msg;
    auto sendTime = fmtSendTime();
    // SendingTime, MsgType, MsgSeqNum, SenderCompID, TargetCompID, Password
    // pre << sendTime << delim << "A" << delim << "1" << delim << aPubKey << delim << "BAXTER" << delim << aPassPhrase;

    // Fix message
    msg << "35=A" << delim << "34=1" << delim << "49=GALAXY-TEST-MD" << delim << "56=BAXTER" << delim
        << "98=0" << delim << "108=30" << delim << "141=Y" << delim << "52=" << sendTime << delim
        // << "554=" << aPassPhrase << delim << "96=" << calculateSignature(aPriKey, pre.str()) << delim 
        << "8013=Y" << delim;

    // Fix Header
    hdr << "8=FIX.4.4" << delim << "9=" << msg.str().size() << delim;
    hdr << msg.str();

    // Fix checksum
    const auto buffer = hdr.str();
    hdr << "10=" << std::setw(3) << std::setfill('0') << generateCheckSum(buffer.c_str(), buffer.size()) << delim;

    // DEBUG.
    const std::string aMsg = hdr.str();
    std::cout << __func__ << ": ";
    for ( const auto &ch : aMsg)
        std::cout << (int(ch) == 1 ? '|' : ch);
    std::cout << std::endl;

    return hdr.str();
}

void
fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    sslSocket stream;
    boost::asio::streambuf buffer;
    boost::asio::streambuf response_;
    char input_buffer_[10 * 1024];

public:
    // Resolver and stream require an io_context
    explicit
    session(boost::asio::io_context& ioc, ssl::context& ctx)
        : resolver_(ioc)
        , stream(ioc, ctx)
    {
    }

    // Start the asynchronous operation
    void
    run() {
        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(stream.native_handle(), aApiHost.c_str())) {
            boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            std::cerr << __func__ << ": " << ec.message() << "\n";
            return;
        }

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
        tcp::resolver::results_type results) {
        if(ec)
            return fail(ec, "resolve");

        // Make the connection on the IP address we get from a lookup
        boost::asio::async_connect(
            stream.next_layer(),
            results.begin(),
            results.end(),
            std::bind(
                &session::on_connect,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    on_connect(boost::system::error_code ec) {
        if(ec)
            return fail(ec, "connect");

        // Perform the SSL handshake
        std::cout << "-------------->Connect Successfull<--------------" << std::endl;

        // Set TCP_NODELAY
        stream.next_layer().set_option(boost::asio::ip::tcp::no_delay(true));

        stream.async_handshake(
            ssl::stream_base::client,
            std::bind(
                &session::on_handshake,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    on_handshake(boost::system::error_code ec) {
        if(ec)
            return fail(ec, "handshake");
        std::cout << "-------------->Handshake Successfull<--------------" << std::endl;
        // std::string msg = orderStatus();
        std::ostream reqStream(&buffer);
        auto const msg = loginRequest();
        reqStream << msg;
        // boost::asio::streambuf()
        boost::asio::async_write(stream, buffer,
            std::bind(
                &session::on_write,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    on_write(
        boost::system::error_code ec,
        std::size_t bytes) {

        if(ec)
            return fail(ec, "write");
        std::cout << "-------------->Write Successfull<--------------" << std::endl;
        std::cout << "[DataSent (" << bytes << ")]." << std::endl;
        // boost::asio::async_read(stream, response_,
        //     std::bind(
        //         &session::on_read,
        //         shared_from_this(),
        //         std::placeholders::_1,
        //         std::placeholders::_2));
        boost::asio::async_read_until(stream, response_, delim,
            std::bind(
                &session::on_read,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    on_read(boost::system::error_code ec,
            std::size_t bytes) {
        if(ec)
            return fail(ec, "read error");

std::cout << __func__ << ": bytes: " << bytes << std::endl;
        std::string data;
        std::istream(&response_) >> data;
std::cout << __func__ << ": data: " << data.size() << std::endl;
        std::cout << __func__ << ": ";
        for ( const auto &ch : data) {
            std::cout << (int(ch) == 1 ? '|' : ch);
        }
        std::cout << std::endl;

        boost::asio::async_read_until(stream, response_, delim,
            std::bind(
                &session::on_read,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }
};

int main() {
    // The io_context is required for all I/O
    boost::asio::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::sslv23_client};

    // This holds the root certificate used for verification
    // load_root_certificates(ctx);

    // Verify the remote server's certificate
    // ctx.set_verify_mode(ssl::verify_peer);
    // ctx.set_verify_mode(boost::asio::ssl::verify_none);

    // Launch the asynchronous operation
    std::make_shared<session>(ioc, ctx)->run();

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return EXIT_SUCCESS;
}