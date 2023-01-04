#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
//#include <boost/bind/bind.hpp>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
#include <chrono>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>
#include <cppcodec/hex_lower.hpp>
#include <string_view>
#include <array>
#include <iterator>
#include <sstream>
#include "./include/parse_balance.hpp"


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//using namespace boost::placeholders;

inline std::string bitfinexSignature (const std::string &key, const std::string &payload){
    unsigned int len = SHA384_DIGEST_LENGTH;
    unsigned char hash[SHA384_DIGEST_LENGTH];

#if OPENSSL_VERSION_NUMBER >= 0x1010106fL
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac,reinterpret_cast<const unsigned char *>(key.c_str()), key.length(), EVP_sha384(), NULL);
    HMAC_Update(hmac,reinterpret_cast<const unsigned char *>(payload.c_str()), payload.length());
    HMAC_Final(hmac, hash, &len);
    HMAC_CTX_free(hmac);
#else
    HMAC_CTX * hmac = HMAC_CTX_new();
    HMAC_Init_ex(hmac,reinterpret_cast<const unsigned char *>(key.c_str()), key.length(), EVP_sha384(), NULL);
    HMAC_Update(hmac,reinterpret_cast<const unsigned char *>(payload.c_str()), payload.length());
    HMAC_Final(hmac, hash, &len);
    HMAC_CTX_free(hmac);
#endif
    return cppcodec::hex_lower::encode(hash, len);
}

inline std::string bitfinexSubscribeChannel(
        const std::string &aSym,
        const std::string &aChannel,
        const unsigned anID) {
    std::stringstream ss;
    ss << "{\"event\": \"subscribe\", "
       << "\"channel\": \"" << aChannel 
       << "\", \"symbol\": \"" << aSym
       << "\", \"id\": " << anID << "}";
    return ss.str();
}

int main(int argc, char** argv)
{
    try
     {
        
        std::stringstream ss;
        auto const host = "api.bitfinex.com";  
        auto const port = "443";
        auto const API_Key = "l1cM7WW2Iph8z0xApK6MPkWfLBpNeariVzVm6h9mLK6";
        auto const API_Secret = "0hW3lkf9NolzEe5vWLiqc0p7AJ1txB4fZsqpTb9nzUN";
        std::ostringstream Nonce;
        Nonce << std::chrono::duration_cast <std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
        // std::string Payload = Nonce;
        std::string Payload = "AUTH" + Nonce.str();
        std::string Auth_Sig = bitfinexSignature(API_Secret, Payload);


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
        ws.handshake(host, "/ws/2");

	// Send auth msg.
        std::string msg = "{ \"event\": \"auth\", \"apiKey\": \"";
        msg = msg + API_Key;
        msg = msg  + "\", \"authSig\": \"";
        msg = msg + Auth_Sig ;
        msg = msg + "\", \"authPayload\": \"";
        msg = msg  + Payload;
        msg = msg  + "\", \"authNonce\": \"";
        msg = msg + Nonce.str() ;
        msg = msg + "\"}";

        // Our message in this case should be stringified JSON-RPC request
        ws.write(net::buffer(std::string(msg)));
        std::cout << "OUT: " << msg <<std::endl;

        // This buffer will hold the incoming message
        beast::flat_buffer buffer;
        std::stringstream oss;
        ws.read(buffer);
        std::cout << "IN: " << beast::make_printable(buffer.data()) << std::endl;

	// subscribe symbols channels. 
        // ORDERS: os: order snapshot, ou: order update, on: order new 
        // POSITIONS: ps: position snapshot, pu: position update, pn: position new, pc: position close
        // TRADES: te: trade execution, tu: trade exec update
        // BALANCE: bu: balance update
        //msg = std::string("{ \"event\": \"ou\", \"symbol\": \"tBTCUSD\", symbol: \"tBTCUSD\" }");
        //msg = std::string("{ \"event\": \"os\", \"symbol\": \"tBTCUSD\", symbol: \"tBTCUSD\" }");
        msg = std::string("{ \"event\": \"bu\", \"symbol\": \"tBTCUSD\", symbol: \"tBTCUSD\" }");
        ws.write(net::buffer(msg));
        std::cout << "OUT: " << msg <<std::endl;
        while(true){
            

            // Read a message into our buffer
            // ws.ReadFrame();
            ws.read(buffer);

            // The make_printable() function helps print a ConstBufferSequence
            std::cout << "IN: " << beast::make_printable(buffer.data()) << std::endl;
            oss << beast::make_printable(buffer.data());
            parse_balance(oss);
       }
        ws.close(websocket::close_code::normal);
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

