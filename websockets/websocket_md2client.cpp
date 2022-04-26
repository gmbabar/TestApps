
// #include "root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
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
class Session : public std::enable_shared_from_this<Session> {
    tcp::resolver resolver_;
    websocket::stream<ssl::stream<tcp::socket>> ws_;
    boost::beast::flat_buffer buffer_;
    std::string m_host;
    std::string m_exchange;
    std::string m_symbol;
    unsigned msg_count_ = 0;

public:
    // Resolver and socket require an io_context
    explicit
    Session(boost::asio::io_context& ioc, ssl::context& ctx)
        : resolver_(ioc)
        , ws_(ioc, ctx) {
    }

    // Start the asynchronous operation
    void
    start(
        char const* host,
        char const* port,
        char const* exchange,
        char const* symbol,
        char const* level) {
        std::cout << "Listener:" << __func__ << std::endl;
        // Save these for later
        m_host = host;
        m_exchange = exchange;
        m_symbol = symbol;
        m_level = level;

        // Look up the domain name
        resolver_.async_resolve(
            host,
            port,
            std::bind(
                &Session::onResolve,
                shared_from_this(),
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
            ws_.next_layer().next_layer(),
            results.begin(),
            results.end(),
            std::bind(
                &Session::onConnect,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    onConnect(boost::system::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "connect");

        // Perform the SSL handshake
        ws_.next_layer().async_handshake(
            ssl::stream_base::client,
            std::bind(
                &Session::onSslHandshake,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    onSslHandshake(boost::system::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "ssl_handshake");

        // Perform the websocket handshake
        ws_.async_handshake(m_host, "/",
            std::bind(
                &Session::onHandshake,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    onHandshake(boost::system::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "handshake");
        
        // Send the message
        ws_.async_write(
            boost::asio::buffer(subscribe(m_exchange, m_symbol, m_level)),
            std::bind(
                &Session::onWrite,
                shared_from_this(),
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
        ws_.async_read(
            buffer_,
            std::bind(
                &Session::onRead,
                shared_from_this(),
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

        std::cout << __func__ << "-" << ++msg_count_ << ": " << boost::beast::buffers_to_string(buffer_.data()) << std::endl;

        buffer_.consume(buffer_.size());


        if (msg_count_ == 50) {
            // Send the message
            ws_.async_write(
                net::buffer(this->unsubscribe("NBINE", "BTCUSDT", "L2|L1")),
                std::bind(
                    &Session::onWrite,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2));
        } 
        else if (msg_count_ == 70) {
                ws_.async_write(
                    net::buffer(text_),
                    std::bind(
                        &Session::onWrite,
                        shared_from_this(),
                        std::placeholders::_1,
                        std::placeholders::_2));
        } 
        else {
                // Read a message into our buffer
                ws_.async_read(
                    buffer_,
                    std::bind(
                        &Session::onRead,
                        shared_from_this(),
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

    // The io_context is required for all I/O
    boost::asio::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::sslv23_client};

    // This holds the root certificate used for verification
    // load_root_certificates(ctx);

    // Launch the asynchronous operation
    std::make_shared<Session>(ioc, ctx)->start(host, port, exchange, symbol, level);

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();

    return EXIT_SUCCESS;
}