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
#include "../include/parser.hpp"


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>



/*
----------subscription msg
{\"type\": \"subscribe\",\"subscriptions\":[{\"name\":\"l2\",\"symbols\":[\"BTCUSD\",\"ETHUSD\",\"ETHBTC\"]}]}
*/

void fail(beast::error_code ec, char const* what) {
    std::cout << what << ":" << ec.message() << std::endl; 
}

class Session : public std::enable_shared_from_this<Session> {

public:
    explicit Session(net::io_context& ioc, ssl::context& ctx)
        :m_resolver(net::make_strand(ioc)),
        m_ws(net::make_strand(ioc), ctx) {
    }

    
    // void run( char const* host, char const* port, char const* target) {
    void run( char const* host, char const* port, char const* msg) {
        std::cout << "Listener:" << __func__ << std::endl;
        m_host = host;
        m_text = msg;
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
        }));

        // Perform the websocket handshake
        m_ws.async_handshake(m_host, "/v2/marketdata", beast::bind_front_handler(&Session::on_handshake, shared_from_this()));
    }

    // void on_handshake(beast::error_code ec) {
    //     std::cout << "Listener:" << __func__ << std::endl;
    //     if(ec)
    //         return fail(ec, "handshake");

    //     m_ws.async_read(m_buffer, beast::bind_front_handler(&Session::on_read, shared_from_this()));
    // }
        void on_handshake(beast::error_code ec) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "handshake");

        // Send the message
        m_ws.async_write(net::buffer(m_text), beast::bind_front_handler(&Session::on_write,shared_from_this()));
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
        // std::cout << __func__ << "-" << ": " << oss.str() << std::endl;
        ParseV2MarketData(oss.str().c_str());
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
    std::string m_text;
};



int main(int argc, char **argv) {
    if(argc < 2 || argc > 2)
    {
        std::cout << "Invalid Arguements :";
        std::cout << "\nExample:";
        // std::cout << "\n\t" << argv[0] << " <target>";
        std::cout << "\n\t" << argv[0] << " <symbols>";
        // std::cout << "\n\t" << argv[0] << " /v1/marketdata/BTCUSD\n\t"<< argv[0] <<" /v1/marketdata/btcusd?top_of_book=true&bids=false\n";
        std::cout << "\n\t" << argv[0] << " \\\"BTCUSD\\\",\\\"ETHUSD\\\",\\\"ETHBTC\\\"\n";
        return -1;
    }
    // Check command line arguments.
    auto const host = "api.gemini.com";
    auto const port = "443";
    char const* symbol = argv[1];
    std::ostringstream oss;
    /*
    ----------subscription msg
    {\"type\": \"subscribe\",\"subscriptions\":[{\"name\":\"l2\",\"symbols\":[\"BTCUSD\",\"ETHUSD\",\"ETHBTC\"]}]}
    {"type":"subscribe","subscriptions":[{"name":"l2","symbols":["BTCUSD"]}]}
    */
    oss << "{\"type\":\"subscribe\",\"subscriptions\":[{\"name\":\"l2\",\"symbols\":[" << argv[1] << "]}]}";
    std::cout << __func__ << ": " << oss.str() << std::endl;
  
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12_client};

    // std::make_shared<Session>(ioc, ctx)->run(host, port, target);
    std::make_shared<Session>(ioc, ctx)->run(host, port, oss.str().c_str());

    ioc.run();
    

    return EXIT_SUCCESS;
}