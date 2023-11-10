#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <iostream>
#include <chrono>
#include <openssl/hmac.h>
#include <cppcodec/hex_lower.hpp>
#include <sstream>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

inline std::string deribitSignature (const std::string &key, const std::string &payload){
    unsigned int len = SHA256_DIGEST_LENGTH;
    unsigned char hash[SHA256_DIGEST_LENGTH];

#if OPENSSL_VERSION_NUMBER >= 0x1010106fL
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac,reinterpret_cast<const unsigned char *>(key.c_str()), key.length(), EVP_sha256(), NULL);
    HMAC_Update(hmac,reinterpret_cast<const unsigned char *>(payload.c_str()), payload.length());
    HMAC_Final(hmac, hash, &len);
    HMAC_CTX_free(hmac);
#else
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac,reinterpret_cast<const unsigned char *>(key.c_str()), key.length(), EVP_sha256(), NULL);
    HMAC_Update(hmac,reinterpret_cast<const unsigned char *>(payload.c_str()), payload.length());
    HMAC_Final(hmac, hash, &len);
    HMAC_CTX_free(hmac);
#endif
    return cppcodec::hex_lower::encode(hash, len);
}


inline std::string deribitAuthReq(std::string apiKey, std::string apiSecret) {
    // Sign the authentication message
    std::ostringstream tss;
    tss << std::chrono::duration_cast <std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    std::string timestamp = tss.str();
    std::string nonce = timestamp;
    std::string data = "";
    std::string signature_payload = timestamp + "\n" + nonce + "\n" + data;
    std::string signature = deribitSignature(apiSecret, signature_payload);

    // Construct authentication message
    std::stringstream authMsg;
    authMsg << R"({"jsonrpc": "2.0",)"
            << R"("id": 1,)"
            << R"("method": "public/auth",)"
            << R"("params": {)"
            << R"( "grant_type": "client_signature",)"
            << R"( "client_id": ")" << apiKey << R"(",)"
            << R"( "timestamp": )" << timestamp << R"(,)"
            << R"( "signature": ")" << signature << R"(",)"
            << R"( "nonce": ")" << nonce << R"(",)"
            << R"( "data": ")" << data << R"(")"
            << R"(}})";
    return authMsg.str();
}

inline std::string deribitBalanceReq() {
    std::ostringstream oss;
    oss << R"({"jsonrpc": "2.0",)"
        << R"("id": 3,)"
        << R"("method": "private/get_account_summary",)"
        << R"("params": {)"
        << R"( "currency": "BTC")"
        << R"(}})";
    return oss.str();
}

int main(int argc, char** argv)
{
    try
     {
        
        std::stringstream ss;
        auto const host = "test.deribit.com";
        auto const port = "443";
        auto const API_Key = "6d3m-Vq7";
        auto const API_Secret = "NuouYQA4KMfZrVdCuC9nKmNJ_HJsrY7r-dDMZZn9lPg";

        // The io_context is required for all I/O
        net::io_context ioc;

        // The SSL context is required, and holds certificates
        ssl::context ctx{ssl::context::sslv23_client};

        // This holds the root certificate used for verification
        //load_root_certificates(ctx);

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        websocket::stream<beast::ssl_stream<tcp::socket>> ws{ioc, ctx};

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        net::connect(ws.next_layer().next_layer(), results.begin(), results.end());

        // Perform the SSL handshake
        ws.next_layer().handshake(ssl::stream_base::client);

        // Optional: Set a decorator to change the User-Agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-client-coro");
            }));

        // Perform the websocket handshake
        ws.handshake(host, "/ws/api/v2");

	// Send auth msg.
        std::string msg = deribitAuthReq(API_Key, API_Secret);

        // Our message in this case should be stringified JSON-RPC request
        ws.write(net::buffer(std::string(msg)));
        std::cout << "OUT: " << msg <<std::endl;

        // This buffer will hold the incoming message
        beast::flat_buffer buffer;
        std::stringstream oss;
        ws.read(buffer);
        std::cout << "IN: " << beast::make_printable(buffer.data()) << std::endl;

	// subscribe symbols channels.TestApps/gemini/gateway/Makefile
        msg = deribitBalanceReq();
        ws.write(net::buffer(msg));
        std::cout << "OUT: " << msg <<std::endl;
        // while(true){

            // Read a message into our buffer
            // ws.ReadFrame();
            ws.read(buffer);

            // The make_printable() function helps print a ConstBufferSequence
            std::cout << "IN: " << beast::make_printable(buffer.data()) << std::endl;
            oss << beast::make_printable(buffer.data());
        // }
        ws.close(websocket::close_code::normal);
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

