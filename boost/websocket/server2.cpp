/**_
 * BLUE FIRE CAPITAL, LLC CONFIDENTIAL
 * _____________________
 * 
 *  Copyright © [2007] - [2012] Blue Fire Capital, LLC
 *  All Rights Reserved.
 * 
 * NOTICE:  All information contained herein is, and remains the property
 * of Blue Fire Capital, LLC and its suppliers, if any.  The intellectual
 * and technical concepts contained herein are proprietary to Blue Fire
 * Capital, LLC and its suppliers and may be covered by U.S. and Foreign
 * Patents, patents in process, and are protected by trade secret or copyright
 * law.  Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained from 
 * Blue Fire Capital, LLC.
_**/

#include <sstream>
#include <iostream>
#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <nebula/asio/asio.hpp>
#include <boost/thread.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include "common/server_certificate.hpp"


using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;               // from <boost/asio/ssl.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>
namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace net = boost::asio;                    // from <boost/asio.hpp>

// Echoes back all received WebSocket messages
struct Md2ClientSession {
    // typedef nebula::func::FastDelegate<void (const std::string&)> DgtClientSocketSend;

    // Take ownership of the socket
    Md2ClientSession(tcp::socket socket, nebula::asio::io_service& aIoc)
        : m_ws(std::move(socket))
        , m_socket(m_ws.next_layer())
        , m_connected(false) {
    }

    void close() {
        m_connected = false;
        // Close the WebSocket connection
        m_ws.async_close(websocket::close_code::normal,
            std::bind(
                &Md2ClientSession::onClose, this,
                std::placeholders::_1));
    }

    void onClose(beast::error_code ec) {}

    // Start the asynchronous operation
    void start() {
        // remote ep
        {
            std::stringstream ss;
            ss << m_socket.remote_endpoint();
            std::cout << __func__ << ": connection from " << ss.str() << std::endl;
        }
        // // Perform the SSL handshake
        // m_ws.next_layer().async_handshake(
        //     ssl::stream_base::server,
        //     std::bind(
        //         &Md2ClientSession::onHandshake, this,
        //         std::placeholders::_1)
        //     );

        // Accept the websocket handshake
        m_ws.async_accept(
            std::bind(
                &Md2ClientSession::onAccept, this,
                std::placeholders::_1)
            );

        // boost::asio::strand<
        //     boost::asio::io_context::executor_type> strand_(m_ws.get_executor());
        // // Accept the websocket handshake
        // m_ws.async_accept(
        //     boost::asio::bind_executor(
        //         strand_,
        //         std::bind(
        //             &Md2ClientSession::onAccept,
        //             this,
        //             std::placeholders::_1)));
    }

    // void onHandshake(boost::system::error_code ec) {
    //     if(ec)
    //         return error(ec, "handshake");

    //     // Accept the websocket handshake
    //     m_ws.async_accept(
    //         std::bind(
    //             &Md2ClientSession::onAccept, this,
    //             std::placeholders::_1)
    //         );
    // }

    void onAccept(boost::system::error_code ec) {
        if(ec)
            return error(ec, "accept");

        // Increase read/send buffers
        {
            std::size_t bufSz = 1024*1024*20;
            m_socket.set_option(boost::asio::socket_base::receive_buffer_size(bufSz));
        }
        {
            std::size_t bufSz = 1024*1024*20;
            m_socket.set_option(boost::asio::socket_base::send_buffer_size(bufSz));
        }

        // Set keep alive
        m_socket.set_option(boost::asio::socket_base::keep_alive(true));

        // Disable Nagle
        m_socket.set_option(boost::asio::ip::tcp::no_delay(true));

        m_connected = true;

        // Read a message
        doRead();
    }

    void doRead() {
        // Read a message into our buffer
        m_ws.async_read(
            m_recvBuffer,
            std::bind(
                &Md2ClientSession::onRead, this,
                std::placeholders::_1,
                std::placeholders::_2)
            );
    }

    void onRead(
        boost::system::error_code ec,
        std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        // This indicates that the Md2ClientSession was closed
        if(ec == websocket::error::closed) {
            close();
            return;
        }

        if(ec) {
            error(ec, "read");
            close();
            return;
        }

        ParseMessage(boost::beast::buffers_to_string(m_recvBuffer.data()));

        // Clear the buffer
        m_recvBuffer.consume(bytes_transferred);

        // Do another read
        doRead();

    }

    void ParseMessage(std::string aMsg) {
        std::cout << "IN: " << aMsg << std::endl;
        socketSend(aMsg);
    }

    void sendNextWsMessage() {
        m_sending = true;
        m_sendingBuffer = m_wsMsgs.front();
        m_wsMsgs.pop_front();
        // std::cout << __func__ << ": sending queued: " << m_sendingBuffer << std::endl;

        m_ws.async_write(
            boost::asio::buffer(m_sendingBuffer),
            std::bind(
                &Md2ClientSession::onWrite, this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void socketSend(const std::string& aMsg) {
        if (! m_connected || m_sending) {
            m_wsMsgs.push_back(aMsg);
            return;
        }
        
        // Send data to the remote host
        m_sending = true;
        m_sendingBuffer = aMsg;
        // std::cout << __func__ << ": sending direct: " << m_sendingBuffer << std::endl;

        m_ws.async_write(
            boost::asio::buffer(m_sendingBuffer),
            std::bind(
                &Md2ClientSession::onWrite, this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    bool sending () {
        return m_sending;
    }

    void onWrite(
        boost::system::error_code ec,
        std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if(ec) {
            std::cout << __func__ << ": Failed: " << ec.message() << std::endl;

            close();
            return error(ec, "write");
        }
        if (!m_wsMsgs.empty()) {
            sendNextWsMessage();
        }
        else {
            m_sending = false;
        }
    }

private:
    // Report a failure
    void error(boost::system::error_code ec, char const* what) {
        std::cerr << what << ": " << ec.message() << "\n";
    }

private:
    websocket::stream<tcp::socket> m_ws;
    tcp::socket &m_socket;
    boost::beast::flat_buffer m_recvBuffer;
    std::string m_sendingBuffer;
    bool m_connected;
    bool m_sending {false};
    std::deque<std::string> m_wsMsgs;
};


// Accepts incoming connections and launches the sessions
struct Md2ClientAcceptor {
    typedef Md2ClientSession session_type;
    typedef std::shared_ptr<session_type>   session_pointer;

    Md2ClientAcceptor(
        boost::asio::io_service& aIoc,
        // ssl::context& aCtx,
        tcp::endpoint aEndpoint)
        : m_ioc(aIoc)
        // , m_ctx(aCtx)
        , m_acceptor(aIoc)
        , m_socket(aIoc) {
        boost::system::error_code ec;

        // Open the acceptor
        m_acceptor.open(aEndpoint.protocol(), ec);
        if(ec) {
            error(ec, "open");
            return;
        }

        // Allow address reuse
        m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if(ec) {
            error(ec, "set_option");
            return;
        }

        // Bind to the server address
        m_acceptor.bind(aEndpoint, ec);
        if(ec) {
            error(ec, "bind");
            return;
        }

        // Start listening for connections
        m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
        if(ec) {
            error(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void start() {
        if(! m_acceptor.is_open()) {
            std::cerr << __func__ << ": " << "acceptor not open" << "\n";
            return;
        }
        doAccept();
    }

    void doAccept() {
        std::cout << "accept: listening " << m_acceptor.local_endpoint() << std::endl;
        m_acceptor.async_accept(
            m_socket,
            std::bind(
                &Md2ClientAcceptor::onAccept, this,
                std::placeholders::_1));
    }

    void onAccept(boost::system::error_code ec) {
        if(ec) {
            error(ec, "accept");
        }
        else {
            // Create the Md2ClientSession and run it
            auto aSessPtr = std::make_shared<Md2ClientSession>(std::move(m_socket), m_ioc);
            aSessPtr->start();
            m_sessions.push_back(aSessPtr);
        }

        // Accept another connection
        doAccept();
    }

private:
    // Report a failure
    void error(boost::system::error_code ec, char const* what) {
        std::cerr << what << ": " << ec.message() << "\n";
    }

private:
    net::io_service& m_ioc;
    // ssl::context& m_ctx;
    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
    std::vector< std::shared_ptr<Md2ClientSession> > m_sessions;
};

int main(int argc, char* argv[])
{
    // Check command line arguments.
    if (argc != 3) {
        std::cerr <<
            "Usage: " << argv[0] << " <address> <port>\n" <<
            "Example:\n" <<
            "    " << argv[0] << " 0.0.0.0 8080\n";
        return EXIT_FAILURE;
    }
    auto const address = boost::asio::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const threads = 1;

    // The io_context is required for all I/O
    boost::asio::io_context ioc{threads};

    // The SSL context is required, and holds certificates
    // ssl::context ctx{ssl::context::tlsv12};

    // This holds the self-signed certificate used by the server
    // load_server_certificate(ctx);

    // Create and launch a listening port
    tcp::endpoint localEp{address, port};
    auto acceptor = std::make_shared<Md2ClientAcceptor>(ioc, localEp);
    acceptor->start();

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
