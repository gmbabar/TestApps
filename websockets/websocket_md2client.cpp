
// #include "root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind/bind.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

using tcp = boost::asio::ip::tcp;                 // from <boost/asio/ip/tcp.hpp>
namespace beast = boost::beast;                   // from <boost/beast.hpp>
namespace http = beast::http;                     // from <boost/beast/http.hpp>
namespace net = boost::asio;                      // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;                 // from <boost/asio/ssl.hpp>
namespace websocket = boost::beast::websocket;    // from <boost/beast/websocket.hpp>

//------------------------------------------------------------------------------

// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

inline std::string subscribe(const std::string& exchange, 
        const std::string& symbol, 
        const std::string& level, 
        const int frequency = 10,
        const int timeWindow = 10,
        const int maxLevels = 10,
        const std::string& bookType = "R",
        const bool oneShot = true) {
    std::ostringstream oss;
    oss << "{"
    << R"("type":"subscribe")"
    << R"(,"exchange":")" << exchange << R"(")"
    << R"(,"symbol":")" << symbol << R"(")"
    << R"(,"level":")" << level << R"(")"
    << R"(,"update_frequency":)"<< frequency 
    << R"(,"time_window_secs":)" << timeWindow
    << R"(,"max_levels":)" << maxLevels
    << R"(,"book_type":")" << bookType << R"(")"
    << R"(,"one_shot":)" << (oneShot ? "true" : "false")
    << "}";
    return oss.str();
}

inline std::string unsubscribe(std::string exchange, 
        std::string symbol, 
        std::string level) {
    std::ostringstream oss;
    oss << "{"
        << R"("type":"unsubscribe")"
        << R"(,"exchange":")" << exchange << R"(")"
        << R"(,"symbol":")" << symbol << R"(")"
        << R"(,"level":")" << level << R"(")"
        << "}";
    return oss.str();
}

// Sends a WebSocket message and prints the response
class Session {
    tcp::resolver resolver_;
    websocket::stream<ssl::stream<tcp::socket>> m_ws;
    boost::beast::flat_buffer buffer_;
    std::string m_host;
    std::string m_port;
    std::string m_exchange;
    std::string m_symbol;
    std::string m_level;
    unsigned m_msgCount = 0;

public:
    // Resolver and socket require an io_service
    explicit
    Session(boost::asio::io_service& ioc, ssl::context& ctx,
            char const* host,
            char const* port,
            char const* exchange,
            char const* symbol,
            char const* level)
            : resolver_(ioc)
            , m_ws(ioc, ctx) {
        m_host = host;
        m_port = port;
        m_exchange = exchange;
        m_symbol = symbol;
        m_level = level;
    }

    // Start the asynchronous operation
    void
    start() {
        std::cout << "Listener:" << __func__ << std::endl;
        // Save these for later

        // Look up the domain name
        resolver_.async_resolve(
            m_host,
            m_port.c_str(),
            std::bind(
                &Session::onResolve,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    onResolve(
        boost::system::error_code ec,
        tcp::resolver::results_type results)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "resolve");

        // Make the connection on the IP address we get from a lookup
        boost::asio::async_connect(
            m_ws.next_layer().next_layer(),
            results.begin(),
            results.end(),
            std::bind(
                &Session::onConnect,
                this,
                std::placeholders::_1));
    }

    void
    onConnect(boost::system::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "connect");

        // Perform the SSL handshake
        m_ws.next_layer().async_handshake(
            ssl::stream_base::client,
            std::bind(
                &Session::onSslHandshake,
                this,
                std::placeholders::_1));
    }

    void
    onSslHandshake(boost::system::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "ssl_handshake");

        // Perform the websocket handshake
        m_ws.async_handshake(m_host, "/",
            std::bind(
                &Session::onHandshake,
                this,
                std::placeholders::_1));
    }

    void
    onHandshake(boost::system::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "handshake");
        
        // Send the message
        m_ws.async_write(
            net::buffer(subscribe(m_exchange, m_symbol, m_level)),
            std::bind(
                &Session::onWrite,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    onWrite(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "write");
        
        // Read a message into our buffer
        m_ws.async_read(
            buffer_,
            std::bind(
                &Session::onRead,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    onRead(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        std::cout << __func__ << "-" << ++m_msgCount << ": " << boost::beast::buffers_to_string(buffer_.data()) << std::endl;

        buffer_.consume(buffer_.size());


        if (m_msgCount == 50) {
            // Send the message
            m_ws.async_write(
                net::buffer(unsubscribe(m_exchange, m_symbol, "L2|L1")),
                std::bind(
                    &Session::onWrite,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));
        } 
        else if (m_msgCount == 70) {
                m_ws.async_write(
                    net::buffer(subscribe(m_exchange, m_symbol, m_level)),
                    std::bind(
                        &Session::onWrite,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2));
        } 
        else {
                // Read a message into our buffer
                m_ws.async_read(
                    buffer_,
                    std::bind(
                        &Session::onRead,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2));
        }
    }

    void
    onClose(boost::system::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "close");

        // If we get here then the connection is closed gracefully

        // The buffers() function helps print a ConstBufferSequence
        // std::cout << boost::beast::buffers(buffer_.data()) << std::endl;
        std::cout << __func__ << std::endl;
    }
};

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    // Check command line arguments.
    if(argc != 6)
    {
        std::cerr <<
            "Usage: " << argv[0] << " <host> <port> <exchange> <symbol> <quote_level>\n" <<
            "Example:\n" <<
            "    " << argv[0] << " localhost 9443 NBINE BTCUSDT \"L1|L2|T|S\" \n";
        return EXIT_FAILURE;
    }
    auto const host = argv[1];
    auto const port = argv[2];
    auto const exchange = argv[3];
    auto const symbol = argv[4];
    auto const level = argv[5];

    // The io_service is required for all I/O
    // boost::asio::io_context ioc;
    boost::asio::io_service ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::sslv23_client};

    // This holds the root certificate used for verification
    // load_root_certificates(ctx);

    // Launch the asynchronous operation
    auto aSessionPtr = std::make_shared<Session>(ioc, ctx, host, port, exchange, symbol, level);
    ioc.post(boost::bind(&Session::start, aSessionPtr));

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();

    return EXIT_SUCCESS;
}