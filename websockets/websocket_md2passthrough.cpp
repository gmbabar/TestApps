
//#include "root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind/bind.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

using tcp = boost::asio::ip::tcp;                 // from <boost/asio/ip/tcp.hpp>
namespace po = boost::program_options;            // from <boost/program_options.hpp>
namespace beast = boost::beast;                   // from <boost/beast.hpp>
namespace http = beast::http;                     // from <boost/beast/http.hpp>
namespace net = boost::asio;                      // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;                 // from <boost/asio/ssl.hpp>
namespace websocket = boost::beast::websocket;    // from <boost/beast/websocket.hpp>

//------------------------------------------------------------------------------

// Report a failure
void fail(boost::system::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

// Sends a WebSocket message and prints the response
class Session {
    tcp::resolver resolver_;
    websocket::stream<ssl::stream<tcp::socket>> m_ws;
    boost::beast::flat_buffer buffer_;
    std::string m_host;
    std::string m_port;
    std::string m_msg;
    unsigned m_msgCount = 0;

public:
    // Resolver and socket require an io_service
    explicit
    Session(boost::asio::io_service& ioc, ssl::context& ctx,
            char const* host,
            char const* port,
            char const* msg)
            : resolver_(ioc)
            , m_ws(ioc, ctx) {
        m_host = host;
        m_port = port;
        m_msg = msg;
    }

    // Start the asynchronous operation
    void start() {
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

    void onResolve(
        boost::system::error_code ec,
        tcp::resolver::results_type results) {
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

    void onConnect(boost::system::error_code ec) {
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

    void onSslHandshake(boost::system::error_code ec) {
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

    void onHandshake(boost::system::error_code ec) {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "handshake");
        
        // Send the message
        m_ws.async_write(
            net::buffer(m_msg),
            std::bind(
                &Session::onWrite,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void onWrite(boost::system::error_code ec,
            std::size_t bytes_transferred) {
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

    void onRead(
        boost::system::error_code ec,
        std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        std::cout << __func__ << "-" << ++m_msgCount << ": " << boost::beast::buffers_to_string(buffer_.data()) << std::endl;

        buffer_.consume(buffer_.size());


        // Read a message into our buffer
        m_ws.async_read(
            buffer_,
            std::bind(
                &Session::onRead,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void onClose(boost::system::error_code ec) {
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

int main(int argc, char** argv) {


    po::options_description desc("Options");
    bool run_mode = false;
    std::string host;
    std::string port;
    std::string msg;
    std::string fileName;
    desc.add_options()
        ("help, help", "produce help me")
        ("host,h", po::value<std::string>(&host), "host to connect")
        ("port,p", po::value<std::string>(&port), "port to listen for clients")
        ("msg,m", po::value<std::string>(&msg), "message to send")
        ("file,f", po::value<std::string>(&fileName), "filename from where to pick messages")
        ;


    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    if(vm.count("help") || 
        (host=="" && port=="" && msg=="") ||
        (host=="" && port=="" && fileName=="")){
            
        std::cout << "Usage:\n\t" <<
        "./websocket_md2passthrough_d --host <host> --port <port> --msg <msg>\n\t"<<
        "./websocket_md2passthrough_d --host <host> --port <port> --file <file>\n\t"<<
        "./websocket_md2passthrough_d --host localhost --port 9443 --msg \"{\\\"type\\\":\\\"security_list_request\\\"}\"\n\t"<<
        "./websocket_md2passthrough_d --host localhost --port 9443 --file msg.txt\n";
        return 1;
    }

    if(msg != "" && fileName !="")
    {
        std::cout << "Usage:\n\t" <<
        "./websocket_md2passthrough_d --host <host> --port <port> --msg <msg>\n\t" <<
        "./websocket_md2passthrough_d --host <host> --port <port> --file <file>\n\t" <<
        "./websocket_md2passthrough_d --host localhost --port 9443 --msg \"{\\\"type\\\":\\\"security_list_request\\\"}\"\n\t" <<
        "./websocket_md2passthrough_d --host localhost --port 9443 --file msg.txt\n";
        std::cout << "\tToo Many Arguements\n";
        return 1;
    }

    // auto const filename = vm["file"].as<const char *>();

    if(host!="" && port != "" && msg != "") std::cout << "Host : " << host << ", Port : " << port << ", msg : " << msg << std::endl;
    if(host!="" && port != "" && fileName != "") std::cout << "Host : " << host << ", Port : " << port << ", fileName : " << fileName << std::endl;



    // The io_service is required for all I/O
    // boost::asio::io_context ioc;
    boost::asio::io_service ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::sslv23_client};

    // This holds the root certificate used for verification
    //load_root_certificates(ctx);

    // Launch the asynchronous operation
    // auto aSessionPtr = std::make_shared<Session>(ioc, ctx, host, port, msg);
    // ioc.post(boost::bind(&Session::start, aSessionPtr));

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();

    return EXIT_SUCCESS;
}


