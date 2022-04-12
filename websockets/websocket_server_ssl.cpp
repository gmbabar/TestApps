
#include "common/server_certificate.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/bind/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

// Report a failure
void fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Echoes back all received WebSocket messages
struct Session : public std::enable_shared_from_this<Session>
{
    // Take ownership of the socket
    Session(tcp::socket&& aSocket, ssl::context& aCtx, net::io_context& anIoc)
        : m_ws(std::move(aSocket), aCtx), m_timer(anIoc)
    {
        // since the whole purpose is to drop session, set call to close session
        // as soon as we start a new session.
        m_timer.expires_from_now( boost::posix_time::seconds(5) );
        m_timer.async_wait(boost::bind(&Session::close, this));
    }

    void close() {
        // Close the WebSocket connection
        m_ws.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Session::onClose,
                shared_from_this()));
    }

    void onClose(beast::error_code ec)
    {
        if(ec)
            return fail(ec, "close");

        // If we get here then the connection is closed gracefully
        std::cout << __func__ << std::endl;
    }

    // Get on the correct executor

    void run()
    {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        net::dispatch(m_ws.get_executor(),
            beast::bind_front_handler(
                &Session::onRun,
                shared_from_this()));
    }

    // Start the asynchronous operation
    
    void onRun()
    {
        // Set the timeout.
        beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

         // Perform the SSL handshake
        m_ws.next_layer().async_handshake(
            ssl::stream_base::server,
            beast::bind_front_handler(
                &Session::onHandshake,
                shared_from_this()));
    }

    
    void onHandshake(beast::error_code ec)
    {
        if(ec)
            return fail(ec, "handshake");

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(m_ws).expires_never();

        // Set suggested timeout settings for the websocket
        m_ws.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        m_ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-async-ssl");
            }));

        // Accept the websocket handshake
        m_ws.async_accept(
            beast::bind_front_handler(
                &Session::onAccept,
                shared_from_this()));
    }

    void onAccept(beast::error_code ec)
    {
        std::cout << __func__ << ": new session" << std::endl;
        if(ec)
            return fail(ec, "accept");

        // Read a message
        doRead();
    }

    void doRead()
    {
        // Read a message into our buffer
        m_ws.async_read(
            m_buffer,
            beast::bind_front_handler(
                &Session::onRead,
                shared_from_this()));
    }

    void onRead(
        beast::error_code ec,
        std::size_t aBytesTransferred)
    {
        std::cout << __func__ << ": " << beast::make_printable(m_buffer.data()) << std::endl;
        boost::ignore_unused(aBytesTransferred);

        // This indicates that the session was closed
        if(ec == websocket::error::closed)
            return;

        if(ec)
            fail(ec, "read");

        // Echo the message
        m_ws.text(m_ws.got_text());
        m_ws.async_write(
            m_buffer.data(),
            beast::bind_front_handler(
                &Session::onWrite,
                shared_from_this()));
    }

    void onWrite(
        beast::error_code ec,
        std::size_t a_BytesTransferred)
    {
        boost::ignore_unused(a_BytesTransferred);

        if(ec)
            return fail(ec, "write");

        // Clear the buffer
        m_buffer.consume(m_buffer.size());

        // Do another read
        doRead();
    }
private:
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> m_ws;
    beast::flat_buffer m_buffer;
    boost::asio::deadline_timer m_timer;
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
struct Listener : public std::enable_shared_from_this<Listener>
{
    Listener(
        net::io_context& aIoc,
        ssl::context& aCtx,
        tcp::endpoint aEndPoint)
        : m_ioc(aIoc)
        , m_ctx(aCtx)
        , m_acceptor(net::make_strand(aIoc))
    {
        std::cout << __func__ << std::endl;
        beast::error_code ec;

        // Open the acceptor
        m_acceptor.open(aEndPoint.protocol(), ec);
        if(ec)
        {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        m_acceptor.set_option(net::socket_base::reuse_address(true), ec);
        if(ec)
        {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        m_acceptor.bind(aEndPoint, ec);
        if(ec)
        {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        m_acceptor.listen(
            net::socket_base::max_listen_connections, ec);
        if(ec)
        {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void run()
    {
        doAccept();
    }

private:
    void doAccept()
    {
        // The new connection gets its own strand
        m_acceptor.async_accept(
            net::make_strand(m_ioc),
            beast::bind_front_handler(
                &Listener::onAccept,
                shared_from_this()));
    }

    
    void onAccept(beast::error_code ec, tcp::socket socket)
    {
        std::cout << __func__ << ": new listener" << std::endl;
        if(ec)
        {
            fail(ec, "accept");
        }
        else
        {
            // Create the session and run it
            std::make_shared<Session>(std::move(socket), m_ctx, m_ioc)->run();
        }

        // Accept another connection
        doAccept();
    }

    net::io_context& m_ioc;
    ssl::context& m_ctx;
    tcp::acceptor m_acceptor;
};

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Check command line arguments.
    if (argc != 4)
    {
        std::cerr <<
            "Usage: " << argv[0] << " <address> <port> <threads>\n" <<
            "Example:\n" <<
            "    " << argv[0] << " 0.0.0.0 8080 1\n";
        return EXIT_FAILURE;
    }
    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const threads = std::max<int>(1, std::atoi(argv[3]));

    // The io_context is required for all I/O
    net::io_context ioc{threads};

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12};

    // This holds the self-signed certificate used by the server
    load_server_certificate(ctx);

    // Create and launch a listening port
    std::make_shared<Listener>(ioc, ctx, tcp::endpoint{address, port})->run();

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back(
        [&ioc]
        {
            ioc.run();
        });
    ioc.run();

    return EXIT_SUCCESS;
}